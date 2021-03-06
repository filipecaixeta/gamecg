#ifndef	MODEL_H
#define MODEL_H
#include <GL/glew.h>
#include <mesh.h>
#include <shader.h>
#include <string>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <camera.h>

using std::string;
using std::vector;

class Model
{
public:
	Model(string path);
	~Model();
	void draw(Shader *shader,bool blending);
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene, aiNode* node);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type);
	GLint TextureFromFile(string filename, string directory);
	vector<Mesh> meshes;
	string directory;
	vec3 cmin;
	vec3 cmax;
};

#endif