# FullDemoTemplateOGL

Demo 3D en C++ que crea una ventana OpenGL 3.3, carga modelos y texturas, reproduce animaciones esqueléticas y permite recorrer una escena con cámara en primera o tercera persona. La escena de ejemplo incluye terreno, cielo, agua, billboards, texto y colisiones entre modelos. Este proyecto intenta implementar los conceptos de POO para facilitar el uso de OpenGL/DirectX.

## Plataformas y renderizado

- En Linux, el punto de entrada usa GLFW y GLAD para crear un contexto OpenGL 3.3 Core.
- En Windows, el punto de entrada usa WinAPI y WGL. El código también contiene una ruta DX11 y un control XInput; la configuración CMake actual construye la ruta OpenGL.
- La interfaz `Mesh` y la fábrica `Shader` separan el código de render de sus implementaciones GL33 y DX11.

## Estructura del código

El código de la aplicación está en `DemoTemplateOGL/DemoTemplateOGL/`:

| Ruta | Responsabilidad |
| --- | --- |
| `DemoTemplateOGL.cpp` | Entrada por plataforma, ciclo principal, callbacks y actualización de entrada/render. |
| `Base/Utilities.h/.cpp` | Tipos compartidos, acciones, temporizador, carga de recursos y logger. |
| `Base/Scene.h` | Interfaz y comportamiento común de actualización de escena, gravedad y colisiones. |
| `Scenario.h/.cpp` | Escena de demostración y sus modelos, terreno, cielo, agua, billboards y textos. |
| `Base/model.h/.cpp` | Carga y gestión de modelos, atributos, transformaciones, animadores y colisiones. |
| `Base/mesh*`, `Base/shader*` | Mallas y shaders; backends OpenGL 3.3 y DX11. |
| `Base/Animation*`, `Base/Bone*`, `Base/Animator*` | Lectura y reproducción de animaciones esqueléticas. |
| `Base/camera.h` | Cámara y seguimiento del modelo principal. |
| `InputDevices/KeyboardInput*` | Teclado, mouse y traducción a acciones del juego. |
| `InputDevices/GamePadRR.h` | Acceso Windows/XInput al gamepad. |
| `KDTree/` | Árbol espacial y pruebas de colisión. |
| `SkyDome.h`, `Terreno.h`, `Water.h`, `Billboard2D.h`, `Texto.*`, `CollitionBox.*` | Elementos de escena y utilidades de presentación. |

El archivo [DemoOGLTemplate.drawio](DemoOGLTemplate.drawio) conserva el diagrama UML original y actualiza sus clases, relaciones y rutas para corresponder con la implementación actual; las exportaciones PDF e imagen están disponibles junto al diagrama.

## Dependencias

El proyecto integra GLFW 3.3.2, GLAD, GLM, Assimp, FreeImage y FreeType. Assimp, GLM y FreeImage están configurados como submódulos; GLFW, GLAD y FreeType están incluidos en el árbol del proyecto.

Clona el repositorio con sus submódulos:

```sh
git clone --recurse-submodules https://github.com/carlosmax3D/FullDemoTemplateOGL.git
cd FullDemoTemplateOGL
```

Si ya tienes el repositorio, inicializa las dependencias con:

```sh
git submodule update --init --recursive
```
o bien si es Windows descargar solo el .zip, debera descargar la dependencia glm de https://github.com/g-truc/glm/tree/2d4c4b4dd31fde06cfffad7915c2b3006402322f y descomprimir el zip en ExternalResources/glm

## Compilar y ejecutar en Linux

Se necesita CMake y un compilador C/C++ disponible. Desde la raíz del repositorio:

```sh
cmake -S . -B build
cmake --build build -j
./runLinux.sh
```

`runLinux.sh` ejecuta el binario desde el directorio de la aplicación para que encuentre los modelos, shaders y texturas con sus rutas relativas. También se puede iniciar manualmente:

```sh
cd DemoTemplateOGL/DemoTemplateOGL
../../build/FullDemoTemplateOGL
```

En Windows se puede abrir `DemoTemplateOGL/DemoTemplateOGL/DemoTemplateOGL.sln` con Visual Studio. La configuración Windows utiliza componentes del SDK de Windows y dependencias con sus bibliotecas importadas.

Para activar DirectX modificar archivo Utilities.h la directiva ENGINE_OPENGL por ENGINE_DIRECTX y compilar el proyecto, sera necesaria la instalacion del DirectXSDK

## Controles

| Control | Acción |
| --- | --- |
| `W` / `S` | Avanzar / retroceder. |
| `A` / `D` | Girar el personaje. |
| `Ctrl` + `A` / `D` | Desplazamiento lateral. |
| `Space` | Saltar. |
| `P` | Alternar primera y tercera persona. |
| `C` | Alternar visualización de hitboxes y estadísticas. |
| Botón izquierdo + movimiento horizontal | Orbitar la cámara alrededor del personaje. |
| Botón derecho + movimiento vertical | Ajustar el ángulo vertical de la cámara. |
| Rueda del mouse | Acercar o alejar la cámara del personaje. |
| `V` + rueda del mouse | Ajustar el zoom de perspectiva. |
| Gamepad XInput en Windows | Stick izquierdo: giro y movimiento adelante/atrás. |

Para informar errores o proponer mejoras, abre un issue en el repositorio.
Mejoras o bugs, favor de levantar un issue.
