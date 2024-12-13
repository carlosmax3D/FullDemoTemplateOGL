# FullDemoTemplateOGL

Este demo contiene lo necesario para hacer una aplicacion usando OpenGL, contiene librerias para el uso de las extensiones de GL, librerias para soporte de
imagenes y una implementacion primitiva para deteccion de colisiones. Este proyecto intenta implementar los conceptos de POO para facilitar el uso de OpenGL.

Para descargar la plantilla con todo y dependencias usar el comando git
git clone --recurse-submodules https://github.com/carlosmax3D/FullDemoTemplateOGL.git

En caso de descargar solo el .zip, debera descargar la dependencia glm de https://github.com/g-truc/glm/tree/2d4c4b4dd31fde06cfffad7915c2b3006402322f y descomprimir el zip en ExternalResources/glm

Entre los archivos mas destacados se tienen:

| File | Dependency | Contains |  
| ----------- | ----------- | ----------- |   
|Utilities.h|N/A|namespace LOGGER (class LOG, class LOGS),Struct Ejes, SCR_WIDTH, SCR_HEIGHT, struct GameActions, Struct Vertex, Struct Texture, s2ws, loadFile, TextureFromFile, namespace UTILITIES_OGL	(Struct ImageDetails, Struct Vertices, Struct Maya, sumaNormal, normaliza, vectoresEsfera, Esfera, Plano, genNormal)|
|KeyboardInput.h|Utilities.h| Class MouseInput, Struct Input	(enum Keys),	KEYS,	KeysEvents,	Init)
|GamePadRR.h|N/A|class GamePadRR|
|material.h|N/A|zeroVec3,	Struct Material, Struct Light|
|shader.h|Utilities.h, material.h|class Shader|
|KDTree.h|N/A|class Node, class KDTree|
|CollitionDetection.h|KDTree.h|SolveEquision,	checkCollision,	print_queue, findCollision|
|mesh.h|Utilities.h, material.h, shader.h, KDTree.h|class Mesh|
|camera.h|Utilities.h|enum Camera_Movement, class Camera|
|Model.h|Logger.h,Utilities.h, material.h, shader.h, mesh.h, camera.h, KDTree.h, CollitionDetection.h|class Model|
|Billboard.h|Model.h, Utilities.h, camera.h, shader.h|class Billboard extends of Model|
|SkyDome.h|Utilities.h, Model.h, camera.h, shader.h|class SkyDome extends of Model|
|Terreno.h|Utilities.h, Model.h, camera.h, shader.h|class Terreno extends of Model|
|Scene.h|camera.h, Model.h, SkyDome.h, Terreno.h, Billboard.h|class Scene|
|Scenario.h|Scene.h|class Scenario|
  
   
Esta plantilla esta estructurada para poder cargar modelos genericos y convencionales de forma rapida y administrada, se pueden generar escenarios individuales
extendiendo o implementando la clase Scene para generar multiples niveles con modelos y diseños diferentes.
  
La plantilla por defecto tiene en su carpeta de binarios la libreria opengl32.dll para forzar el render por medio de Mesa en lugar de usar la 
grafica de la computadora, si tu grafica soporta las extensiones de OGL 2 en delante, puedes remover el archivo, si necesitas extensiones superiores de OGL
te invito a modificar el archivo principal del WinMain para agregar las extensiones faltantes a tu ventana.  

La plantilla tiene soporta para gamepad pero no tiene mapeo a las funciones de la plantilla,
funciones con teclado ya se encuentran mapeadas:

| Command | Action | 
| ----------- | ----------- |
|W,A,S,D | movimiento de camera y modelo principal|
|SHIFT + (A,D) | movimiento lateral|
|MouseLeftClick + MovimientoMouseX | Rotacion de camara sobre personaje|
|MouseRightClick + MovimientoMouseT | Posicion de camara sobre personaje|
|DownMouseWheel | Zoom out Personaje|
|UpMouseWheel | Zoom in Personaje|
|V + DownMouseWheel | Zoom out Perspectiva|
|V + UpMouseWheel | Zoom in Perspectiva|
|P | Switch first person - third person|


Mejoras o bugs, favor de levantar un issue.
