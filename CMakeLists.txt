cmake_minimum_required(VERSION 3.19)
project(Exercise6)

add_executable(COSC422Assignment1.out
        loadTGA.h
        TerrainPatches.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} )

target_link_libraries( COSC422Assignment1.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} ${GLM_LIBRARY} )
