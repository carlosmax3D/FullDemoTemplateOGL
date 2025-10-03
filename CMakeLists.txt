cmake_minimum_required(VERSION 3.5.0)
project(FullDemoTemplateOGL VERSION 0.1.0)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_subdirectory(${CMAKE_SOURCE_DIR}/ExternalResources/glfw-3.3.2/)
add_subdirectory(${CMAKE_SOURCE_DIR}/ExternalResources/glm/)
add_subdirectory(${CMAKE_SOURCE_DIR}/ExternalResources/assimp/)
add_subdirectory(${CMAKE_SOURCE_DIR}/ExternalResources/FreeImage/)
add_subdirectory(${CMAKE_SOURCE_DIR}/ExternalResources/freetype/)

include_directories(FullDemoTemplateOGL
    ${CMAKE_SOURCE_DIR}/ExternalResources/assimp/include/
    ${CMAKE_SOURCE_DIR}/ExternalResources/glfw-3.3.2/include/
    ${CMAKE_SOURCE_DIR}/ExternalResources/glad/include/
    ${CMAKE_SOURCE_DIR}/ExternalResources/glm/
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/
    ${CMAKE_SOURCE_DIR}/ExternalResources/FreeImage/Source/
    ${CMAKE_SOURCE_DIR}/ExternalResources/freetype/include/
    )
link_directories(FullDemoTemplateOGL
    ${CMAKE_SOURCE_DIR}/ExternalResources/glfw-3.3.2/build/src/
    ${CMAKE_SOURCE_DIR}/ExternalResources/freetype/
    )

add_executable(FullDemoTemplateOGL 
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/Base/glad.c
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/Base/model.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/Base/Utilities.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/Base/Animation.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/Base/Animator.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/Base/Bone.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/InputDevices/KeyboardInput.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/KDTree/KDTree.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/KDTree/CollitionDetection.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/Base/Billboard.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/Base/font_atlas.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/RainSystem.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/CollitionBox.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/Texto.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/Scenario.cpp
    ${CMAKE_SOURCE_DIR}/DemoTemplateOGL/DemoTemplateOGL/DemoTemplateOGL.cpp
)

target_link_libraries(FullDemoTemplateOGL glfw)
target_link_libraries(FullDemoTemplateOGL glm)
target_link_libraries(FullDemoTemplateOGL FreeImage)
target_link_libraries(FullDemoTemplateOGL freetype)
target_link_libraries(FullDemoTemplateOGL assimp)
#find_package(assimp REQUIRED)
#if (assimp_FOUND)
#    set(ASSIMP_LIBRARY "assimp")
#    add_library(${ASSIMP_LIBRARY} SHARED IMPORTED)
#    set_target_properties(${ASSIMP_LIBRARY} PROPERTIES IMPORTED_LOCATION "${LIB_DIR}/assimp/libassimp.so")
#    target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARIES})
#endif(assimp_FOUND)
