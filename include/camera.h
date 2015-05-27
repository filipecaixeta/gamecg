#ifndef	CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
using namespace glm;

class Camera
{
public:
	Camera();
	Camera(vec3 eyePos,vec3 lookAtPos,int w,int h);
	~Camera();
	void loadToShader(GLuint program,mat4 &model);
	void setAspect(int w,int h);
	void update();
	vec3 eyePos;
	vec3 lookAtPos;
	vec3 up;
	float aspect;
	float near;
	float far;
	mat4 Projection;
	mat4 View;
};

#endif