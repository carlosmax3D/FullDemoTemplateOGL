
#include "font_atlas.h"
#include "Utilities.h"

font_atlas *font_atlas::fontsLoaded = NULL;
float font_atlas::oglVersion = 0;

void font_atlas::clearInstance(){
	while (fontsLoaded != NULL){
		font_atlas *actual = fontsLoaded;
		fontsLoaded = fontsLoaded->next;
		delete actual;
	}
}

font_atlas *font_atlas::find(const char* fontName){
	font_atlas *idx = font_atlas::fontsLoaded;
	while (idx != NULL){
		if (strcmp(idx->name,fontName) == 0)
			return idx;
		idx = idx->next;
	}
	return NULL;
}

font_atlas &font_atlas::getInstance(){
	const char *defFont = "shaders/ttf_FreeSans.ttf";
	return getInstance(defFont);
}

font_atlas &font_atlas::getInstance(const char* fontName){
	const char* defFont = "shaders/ttf_CanadaDBNormal.ttf";
	char* font = (char*)fontName;	
	if (strcmp(fontName,"") == 0 || strlen(fontName) < 1)
		font = (char*)defFont;
	font_atlas *fontFound = font_atlas::find(font);
	if (fontFound != NULL){
		return *fontFound;
	} else {
		font_atlas *n = new font_atlas();
		if (fontsLoaded != NULL)
			fontsLoaded->prev = n;
		n->next = fontsLoaded;
		fontsLoaded = n;
		fontsLoaded->create_atlas(font);
		return *n;
	}
}

font_atlas::font_atlas()
	:textureID(0), TextureWidth(0), TextureHeight(0)
{
	if (font_atlas::oglVersion == 0){
		const char* version = (const char*)glGetString(GL_VERSION);
		font_atlas::oglVersion = atof(version);	
	}
}

font_atlas::~font_atlas() {
	glDeleteTextures(1, &textureID);
	textureID = 0;
}

void font_atlas::create_atlas(){
	const char *defFont = "shaders/ttf_FreeSans.ttf";
	create_atlas(defFont);
}
void font_atlas::create_atlas(const char *fontName) {
	// FreeType
	// --------
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		ERRORL("ERROR::FREETYPE: Could not init FreeType Library", "Error");
		return;
	}

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, fontName, 0, &face)) {
		ERRORL("ERROR::FREETYPE: Failed to load font", "Error");
		return;
	} else {
#ifdef _WIN32 
		strcpy_s(this->name, 100, fontName);
#else
		strcpy(this->name, fontName);
#endif
		// Clear the previous map (if any)
		ch_atlas.clear();

		// set the font size
		FT_Set_Pixel_Sizes(face, 0, 128);

		// Below line keep the unpack alignment otherwise fonts will be skewed
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// initialize variables to keep track of texture atlas size
		int atlas_width = 0;
		int atlas_height = 0;

		// loop through the characters and load their glyphs
		for (unsigned char c = 0; c < 128; c++)
		{
			// load glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				ERRORL("ERROR::FREETYTPE: Failed to load Glyph", "Error");
				continue;
			}

			// update atlas size variables
			atlas_width += face->glyph->bitmap.width;
			atlas_height = std::max(atlas_height, static_cast<int>(face->glyph->bitmap.rows));
		}

		// generate texture for the atlas
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			atlas_width,
			atlas_height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			nullptr
		);

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// initialize x position for next glyph
		int x = 0;

		// loop through the characters again and add their glyphs to the atlas
		for (unsigned char c = 0; c < 128; c++)
		{
			// load glyph
			if (FT_Load_Char(face, static_cast<char>(c), FT_LOAD_RENDER))
			{
				ERRORL("ERROR::FREETYTPE: Failed to load Glyph", "Error");
				continue;
			}

			// add glyph to texture atlas
			if (font_atlas::oglVersion >= 4.5f && glTextureSubImage2D != NULL)
				glTextureSubImage2D(
					textureID,
					0,
					x,
					0,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
			else{
				glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					x,
					0,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
			}

			// store glyph information in character map
			Character character;

			character.Size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			character.Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
			character.Advance = face->glyph->advance.x;

			// compute glyph texture coordinates
			character.top_left.x = static_cast<float>(x) / static_cast<float>(atlas_width);
			character.top_left.y = 0.0f;
			character.bot_right.x = static_cast<float>(x + character.Size.x) / static_cast<float>(atlas_width);
			character.bot_right.y = static_cast<float>(character.Size.y) / static_cast<float>(atlas_height);

			ch_atlas.emplace(std::pair<char, Character>(c, character));

			// update x position for next glyph
			x += face->glyph->bitmap.width;
		}
	}

	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}