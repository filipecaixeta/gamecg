#include <camera.h>

Camera::Camera(vec3 eyePos,vec3 lookAtPos,int w,int h):
	eyePos(eyePos),
	lookAtPos(lookAtPos),
	up(0.0f,1.0f,0.0f)
{
	near=0.1f;
	far=300.0f;
	setAspect(w,h);
}
Camera::~Camera()
{

}
void Camera::loadToShader(GLuint program,mat4 &Model)
{
	glUniform3fv(glGetUniformLocation(program, "eyePos"), 1, &eyePos[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "Projection")
		, 1, GL_FALSE, &Projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "View")
		, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "Model")
		, 1, GL_FALSE, &Model[0][0]);
}
void Camera::setAspect(int w,int h)
{
	aspect=(float)w/(float)h;
	update();
}
void Camera::update()
{
	Projection = glm::perspective(45.0f, aspect, near, far);
	View       = glm::lookAt(
	    eyePos,
	    lookAtPos,
	    up
	);
}