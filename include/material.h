#ifndef	MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using glm::vec4;

class Material
{
public:
	Material(aiColor4D _color_diffuse, aiColor4D _color_specular,
			 aiColor4D _color_ambient, aiColor4D _color_emissive,
			 aiColor4D _color_transparent, float _opacity, float _shininess);
	Material(aiMaterial *mat);
	void loadToShader(GLuint program);
	void print();
	~Material();
	vec4 color_diffuse;
	vec4 color_specular;
	vec4 color_ambient;
	vec4 color_emissive;
	vec4 color_transparent;
	float opacity;
	float shininess;
};

#endif