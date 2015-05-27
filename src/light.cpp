#include <light.h>

Light::Light(vec3 _position):
	Lpos(_position,0.0f),
	ambient(0.6f,0.6f,0.6f,1.0f),
	diffuse(0.3f,0.3f,0.3f,1.0f),
	specular(0.7f,0.7f,0.7f,1.0f)
{
	
	
}
Light::~Light()
{
	
}
void Light::loadToShader(GLuint program)
{
	glUniform4fv(glGetUniformLocation(program, "lightS.position"), 1, &Lpos[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.ambient"), 1, &ambient[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.diffuse"), 1, &diffuse[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.specular"), 1, &specular[0]);
}
mat4 Light::getMatrix()
{
	return mat4(1.0);
}