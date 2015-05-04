#ifndef	MESH_H
#define MESH_H

#include <vector>
#include <vertex.h>
#include <texture.h>
#include <GL/glew.h>
#include <shader.h>
using std::vector;

class Mesh
{
	public:
		Mesh(vector<Vertex> vertices,vector<GLuint> indices,vector<Texture> textures);
		~Mesh();
		void draw(Shader *shader);
		vector<Vertex>	vertices;
		vector<GLuint>	indices;
		vector<Texture>	textures;
	private:
		GLuint vertexArrayObject;
		GLuint vertexBufferObject;
		GLuint elementBufferObject;
};

#endif