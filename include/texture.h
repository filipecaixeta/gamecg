#ifndef	TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>
using std::string;

#define TYPE_DIFFUSE 0
#define TYPE_SPECULAR 1

struct Texture
{
	GLuint id;
	int type;
	string path;
};

#endif