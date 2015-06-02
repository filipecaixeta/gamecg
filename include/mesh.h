#ifndef	MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <vertex.h>
#include <texture.h>
#include <GL/glew.h>
#include <shader.h>
#include <material.h>
using std::vector;
using std::string;
using glm::mat4;
using glm::vec3;

class Mesh
{
	public:
		Mesh(vector<Vertex> vertices,vector<GLuint> indices,vector<Texture> textures);
		~Mesh();
		void draw(Shader *shader);
		vector<Vertex>	vertices;
		vector<GLuint>	indices;
		vector<Texture>	textures;
		string meshName;
		mat4 transformation;
		Material *material;
		vec3 center;
		vec3 cmin;
		vec3 cmax;
	private:
		GLuint vertexArrayObject;
		GLuint vertexBufferObject;
		GLuint elementBufferObject;
};

#endif