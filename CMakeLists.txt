cmake_minimum_required (VERSION 2.8)
project (GP2BaseCode)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -ftest-coverage -fprofile-arcs")

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(SDL2_Image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(FBX REQUIRED)

add_executable(GP2BaseCode src/main.cpp 
							src/Graphics.cpp 
							src/Shader.cpp 
							src/Texture.cpp 
							src/FileSystem.cpp
							src/FBXLoader.cpp 
							include/Common.h 
							include/Graphics.h 
							include/Vertices.h
							include/Shader.h 
							include/Texture.h 
							include/FileSystem.h 
							include/Mesh.h
							include/FBXLoader.h
							assets/shaders/TextureFS.glsl 
							assets/shaders/textureVG.glsl
							assets/shaders/ambientVS.glsl
							assets/shaders/ambientFS.glsl
							assets/shaders/diffuseVS.glsl
							assets/shaders/diffuseFS.glsl
							assets/shaders/specularVS.glsl
							assets/shaders/specularFS.glsl)

include_directories(GP2BaseCode include/ 
							${OPENGL_INCLUDE_DIRS} 
							${SDL2_INCLUDE_DIR} 
							${GLEW_INCLUDE_DIRS} 
							${GLM_INCLUDE_DIRS} 
							${SDL2IMAGE_INCLUDE_DIR} 
							${SDL2_TTF_INCLUDE_DIR}
							${FBX_INCLUDE_DIR})
# create the program
target_link_libraries(GP2BaseCode 
							${SDL2_LIBRARY} 
							${OPENGL_LIBRARIES} 
							${GLEW_LIBRARIES} 
							${SDL2IMAGE_LIBRARY} 
							${SDL2_TTF_LIBRARIES} 
							${FBX_LIBRARY})

if (MSVC)
add_custom_command(TARGET GP2BaseCode  POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/dlls $<TARGET_FILE_DIR:GP2BaseCode>)
endif (MSVC)

add_custom_command(TARGET GP2BaseCode  PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                 ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:GP2BaseCode>/assets)
