#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <map>
#include <GL/glew.h>

class Shader
{
public:
	Shader(const std::string vertexShaderFile, const std::string fragmentShaderFile);
	void createProgram();
	void use(bool b);
	GLuint getProgram();
	void reloadShader();
	~Shader();
protected:
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;
private:
	std::string vertexShaderFile;
	std::string fragmentShaderFile;
	time_t vsmodified;
	time_t fsmodified;
};

#endif