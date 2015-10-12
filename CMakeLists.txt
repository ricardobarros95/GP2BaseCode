cmake_minimum_required (VERSION 2.8)
project (GP2BaseCode)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -ftest-coverage -fprofile-arcs")

find_package(SDL2 REQUIRED)

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)

find_package(GLM REQUIRED)

add_executable(GP2BaseCode src/main.cpp src/Graphics.cpp include/Common.h include/Graphics.h include/Shader.h src/Shader.cpp)
include_directories(GP2BaseCode include/ ${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})
target_link_libraries(GP2BaseCode ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})



if (MSVC)
add_custom_command(TARGET GP2BaseCode POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/dlls $<TARGET_FILE_DIR:GP2BaseCode>)

endif (MSVC)


add_custom_command(TARGET GP2BaseCode POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/assets
		$<TARGET_FILE_DIR:GP2BaseCode>/assets)