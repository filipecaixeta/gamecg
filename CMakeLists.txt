cmake_minimum_required(VERSION 2.8)

project(main)

#########################################################
# COMPILATION DIRECTIVES
#########################################################
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASSIMP_CXX_FLAGS} -fms-extensions -std=c++11 ")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()


#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    include_directories(${OpenGL_INCLUDE_DIRS})
    link_directories(${OpenGL_LIBRARY_DIRS})
    add_definitions(${OpenGL_DEFINITIONS})
else()
    message(ERROR " OPENGL not found!")
endif()


#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
if (GLUT_FOUND)
    include_directories(${GLUT_INCLUDE_DIRS})
    link_directories(${GLUT_LIBRARY_DIRS})
    add_definitions(${GLUT_DEFINITIONS})
else()
    message(ERROR " GLUT not found!")
endif()

#########################################################
# FIND GLEW
#########################################################
find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_directories(${GLEW_LIBRARY_DIRS})
    add_definitions(${GLEW_DEFINITIONS})
else()
    message(ERROR " GLEW not found!")
endif()

#########################################################
# FIND ASSIMP
#########################################################
find_package(ASSIMP REQUIRED)
if (ASSIMP_FOUND)
    include_directories(${ASSIMP_INCLUDE_DIRS})
else()
    message(ERROR " ASSIMP not found!")
endif()

#########################################################
# FIND SDL 2
#########################################################
FIND_LIBRARY(SDL2_LIBRARY SDL2 /usr/local/lib/libSDL2.so /usr/lib/libSDL2.so)
FIND_LIBRARY(SDL2_MIXER SDL2_mixer /usr/local/lib/libSDL2_mixer.so  /usr/lib/libSDL2_mixer.so)


#########################################################
# INCLUDE DIRECTORIES
#########################################################
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/include 
${ASSIMP_INCLUDE_DIRS}
${BULLET_PHYSICS_SOURCE_DIR}/src 
/usr/local/include/bullet
/usr/local/include/SDL2
/usr/include/bullet
/usr/include/SDL2
)

#########################################################
# FIND BULLET
#########################################################
FIND_LIBRARY( BULLET_DYNAMICS BulletDynamics /usr/local/lib/libBulletDynamics)
FIND_LIBRARY( BULLET_COLLISION BulletCollision /usr/local/lib/libBulletCollision)
FIND_LIBRARY( LINEAR_MATH LinearMath /usr/local/lib/libLinearMath)
FIND_LIBRARY( OPENGL_SUP OpenGLSupport /usr/local/lib/libOpenGLSupport)

#  ASSIMP_DIR - The preferred installation prefix for searching for ASSIMP. Set by the user.
#
# ASSIMP_ROOT_DIR - the root directory where the installation can be found
# ASSIMP_CXX_FLAGS - extra flags for compilation
# ASSIMP_LINK_FLAGS - extra flags for linking
# ASSIMP_INCLUDE_DIRS - include directories
# ASSIMP_LIBRARY_DIRS - link directories
# ASSIMP_LIBRARIES - libraries to link plugins with

find_library(ASSIMP2 assimp /usr/local/lib/libassimp)

#########################################################
# INCLUDE FILES
#########################################################
set(SOURCES 
	src/model.cpp
	src/mesh.cpp
	src/shader.cpp
	src/material.cpp
	src/car.cpp
	src/scenario.cpp
	include/kdtree.hpp
	src/camera.cpp
	src/light.cpp
	src/vehiclePhysic.cpp
	src/DemoApplication.cpp
	src/GlutDemoApplication.cpp
	src/SFX.cpp
	src/music.cpp
	src/chunk.cpp
	src/hud.cpp
	src/stb_image.c)

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

#########################################################
# LLINKING & STUFF
#########################################################
set(ALL_LIBS
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
	${GLEW_LIBRARIES}
	${BULLET_DYNAMICS}
	${BULLET_COLLISION}
	${LINEAR_MATH}
	${OPENGL_SUP}
	${ASSIMP_LIBRARY}
	${ASSIMP2}
	${SDL2_LIBRARY}
	${SDL2_MIXER}
)

target_link_libraries(${PROJECT_NAME} ${ALL_LIBS} assimp)

IF (INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
			SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES  DEBUG_POSTFIX "_Debug")
			SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES  MINSIZEREL_POSTFIX "_MinsizeRel")
			SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES  RELWITHDEBINFO_POSTFIX "_RelWithDebugInfo")
ENDIF(INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
