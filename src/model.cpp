#include <model.h>
#include <iostream>
#include <glm/glm.hpp>
#include <stb_image.h>

using glm::vec3;
using glm::vec2;
using std::string;

Model::Model(string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR: ASSIMP " << importer.GetErrorString()<< std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
		processMesh(scene->mMeshes[node->mMeshes[i]], scene);
	for(unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene);
}
void Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);
    vector<GLuint> indices;
    vector<Texture> textures;

    for(GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vec3 vecTemp;

        vecTemp.x = mesh->mVertices[i].x;
        vecTemp.y = mesh->mVertices[i].y;
        vecTemp.z = mesh->mVertices[i].z;
        vertex.Position = vecTemp;

        vecTemp.x = mesh->mNormals[i].x;
        vecTemp.y = mesh->mNormals[i].y;
        vecTemp.z = mesh->mNormals[i].z;
        vertex.Normal = vecTemp;
 
 		// Not considering more than 1 texture
        if(mesh->mTextureCoords[0])
        {
            vec2 vecTemp2;
            vecTemp2.x = mesh->mTextureCoords[0][i].x; 
            vecTemp2.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vecTemp2;
        }
        else
            vertex.TexCoords = vec2(0.0f, 0.0f);
        vertices.push_back(vertex);
    }
    
    for(GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if((signed)mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // 1. Diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. Specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    
    meshes.push_back(Mesh(vertices, indices, textures));
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
    vector<Texture> textures;
    for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), directory);
        texture.type = type;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }
    return textures;
}
GLint Model::TextureFromFile(string filename, string directory)
{
     //Generate texture ID and load texture data 
    filename = directory + '/' + filename;
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height,numComponents;
    unsigned char *image = stbi_load(filename.c_str(),&width, &height,&numComponents,4);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);	

    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}
Model::~Model()
{

}
void Model::draw(Shader *shader)
{
	for (Mesh& mesh: meshes)
		mesh.draw(shader);
}