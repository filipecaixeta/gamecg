cmake_minimum_required(VERSION 2.8)

project(main)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASSIMP_CXX_FLAGS} -Wall -Wextra -fms-extensions -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIRS})
endif()
find_package(GLUT REQUIRED)
if (GLUT_FOUND)
    include_directories(${GLUT_INCLUDE_DIRS})
endif()
find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
endif()
find_package(ASSIMP REQUIRED)
if (ASSIMP_FOUND)
    include_directories(${ASSIMP_INCLUDE_DIRS})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${ASSIMP_INCLUDE_DIRS})

#  ASSIMP_DIR - The preferred installation prefix for searching for ASSIMP. Set by the user.
#
# ASSIMP_ROOT_DIR - the root directory where the installation can be found
# ASSIMP_CXX_FLAGS - extra flags for compilation
# ASSIMP_LINK_FLAGS - extra flags for linking
# ASSIMP_INCLUDE_DIRS - include directories
# ASSIMP_LIBRARY_DIRS - link directories
# ASSIMP_LIBRARIES - libraries to link plugins with

set(SOURCES 
	src/model.cpp
	src/mesh.cpp
	src/shader.cpp
	src/material.cpp
	src/stb_image.c)

add_executable(main main.cpp ${SOURCES})

target_link_libraries( main ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${ASSIMP_LIBRARY} assimp)