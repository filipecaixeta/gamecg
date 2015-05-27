#ifndef	LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>
#include <GL/glew.h>
using namespace glm;

class Light
{
public:
	Light(vec3 _position);
	~Light();
	void loadToShader(GLuint program);
	mat4 getMatrix();

	vec4 Lpos;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 direction;
};

#endif