
#include "font_atlas.h"
#include "Utilities.h"
#ifndef _WIN32 
#define strcpy_s(x, y, z) strcpy(x, z);
#endif

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
	: TextureWidth(0), TextureHeight(0)
{
	if (font_atlas::oglVersion == 0){
#ifdef ENGINE_DIRECTX
		const char* version = "11";
#else
		const char* version = (const char*)glGetString(GL_VERSION);
#endif
		font_atlas::oglVersion = atof(version);	
	}
}

font_atlas::~font_atlas() {
#ifdef ENGINE_DIRECTX
	if (texture.idDX11 != NULL) texture.idDX11->Release();
	texture.idDX11 = NULL;
#else
	glDeleteTextures(1, &texture.id);
	texture.id = 0;
#endif
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
	if (FT_Init_FreeType(&ft)) {
		ERRORL("ERROR::FREETYPE: Could not init FreeType Library", "Error");
		return;
	}
	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, fontName, 0, &face)) {
		ERRORL("ERROR::FREETYPE: Failed to load font", "Error");
		return;
	} else {
		strcpy_s(this->name, 100, fontName);
		// Clear the previous map (if any)
		ch_atlas.clear();
		// set the font size
		FT_Set_Pixel_Sizes(face, 0, 128);
#ifndef ENGINE_DIRECTX
		// Below line keep the unpack alignment otherwise fonts will be skewed
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#endif
		// initialize variables to keep track of texture atlas size
		int atlas_width = 0;
		int atlas_height = 0;
		// loop through the characters and load their glyphs
		for (unsigned char c = 0; c < 128; c++) {
			// load glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				ERRORL("ERROR::FREETYTPE: Failed to load Glyph", "Error");
				continue;
			}
			// update atlas size variables
			atlas_width += static_cast<int>(face->glyph->bitmap.width);
			atlas_height = std::max(atlas_height, static_cast<int>(face->glyph->bitmap.rows));
		}
		TextureWidth = atlas_width;
		TextureHeight = atlas_height;
#ifdef ENGINE_DIRECTX
		ID3D11Texture2D* texture = nullptr;
		D3D11_TEXTURE2D_DESC desc = {0};
		desc.Width = atlas_width;
		desc.Height = atlas_height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8_UNORM;   // GL_RED
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = 0;
		device->CreateTexture2D(&desc, nullptr, &texture);
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
#else
		// generate texture for the atlas
		glGenTextures(1, &texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
		// initialize x position for next glyph
		int x = 0;
		// loop through the characters again and add their glyphs to the atlas
		for (unsigned char c = 0; c < 128; c++) {
			// load glyph
			if (FT_Load_Char(face, static_cast<char>(c), FT_LOAD_RENDER)) {
				ERRORL("ERROR::FREETYTPE: Failed to load Glyph", "Error");
				continue;
			}
			int w = static_cast<int>(face->glyph->bitmap.width);
			int h = face->glyph->bitmap.rows;
			int pitch = static_cast<int>(face->glyph->bitmap.pitch);
			unsigned char* src = face->glyph->bitmap.buffer;
#ifdef ENGINE_DIRECTX
			if (w > 0 && h > 0) {
				std::vector<uint8_t> temp(w * h);
				for (int row = 0; row < h; ++row)
					memcpy(&temp[row * w], src + row * pitch, w);
				D3D11_BOX box{};
				box.left = x;
				box.right = x + w;
				box.top = 0;
				box.bottom = h;
				box.front = 0;
				box.back = 1;
				ctx->UpdateSubresource(texture, 0, &box, temp.data(), w, 0);
			}
#else
			// add glyph to texture atlas
			if (font_atlas::oglVersion >= 4.5f && glTextureSubImage2D != NULL)
				glTextureSubImage2D(texture.id, 0, x, 0, w, h, GL_RED, GL_UNSIGNED_BYTE, src);
			else
				glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, w, h, GL_RED, GL_UNSIGNED_BYTE, src);
#endif
			// store glyph information in character map
			Character character;
			character.Size = glm::ivec2(w, h);
			character.Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
			character.Advance = face->glyph->advance.x;
			// compute glyph texture coordinates
			character.top_left.x = static_cast<float>(x) / static_cast<float>(atlas_width);
			character.top_left.y = 0.0f;
			character.bot_right.x = static_cast<float>(x + character.Size.x) / static_cast<float>(atlas_width);
			character.bot_right.y = static_cast<float>(character.Size.y) / static_cast<float>(atlas_height);
			ch_atlas.emplace(std::pair<char, Character>(c, character));
			// update x position for next glyph
			x += w;
		}
#ifdef ENGINE_DIRECTX
		device->CreateShaderResourceView(texture, &srvDesc, &this->texture.idDX11);
		texture->Release();
#endif
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}