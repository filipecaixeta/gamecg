#ifndef	VERTEX_H
#define VERTEX_H
#include <glm/glm.hpp>
using namespace glm;

struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
};

#endif