#include <model.h>
#include <iostream>
#include <glm/glm.hpp>
#include <stb_image.h>

using glm::mat4;
using glm::vec3;
using glm::vec2;
using std::string;

Model::Model(string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(), 
        aiProcess_Triangulate | aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals );

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
		processMesh(scene->mMeshes[node->mMeshes[i]], scene,node);
	for(unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene);
}
void Model::processMesh(aiMesh* mesh, const aiScene* scene, aiNode* node)
{
    vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);
    vector<GLuint> indices;
    indices.reserve(mesh->mNumFaces*3);
    vector<Texture> textures;

    int numUV = mesh->mNumUVComponents[0];

    aiMatrix4x4 t=node->mTransformation;
    mat4 tr=mat4(t.a1,t.a2,t.a3,t.a4,t.b1,t.b2,t.b3,t.b4,t.c1,t.c2,t.c3,t.c4,t.d1,t.d2,t.d3,t.d4);
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

 
        if (mesh->mTangents)
        {
            vecTemp.x = mesh->mTangents[i].x;
            vecTemp.y = mesh->mTangents[i].y;
            vecTemp.z = mesh->mTangents[i].z; 
        }
        else
        {
            vecTemp.x = 1;
            vecTemp.y = vecTemp.z = 0;
        }
        vertex.Tangent = vecTemp;

        if (mesh->mColors[0])
        {
            vecTemp.x = mesh->mColors[0][i].r;
            vecTemp.y = mesh->mColors[0][i].g;
            vecTemp.z = mesh->mColors[0][i].b; 
        }
        else
        {
            vecTemp.x = vecTemp.y = vecTemp.z = 0.5;
        }
        vertex.Color = vecTemp;

        if(numUV==2)
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

    // std::cout << "mMaterialIndex" << mesh->mMaterialIndex << std::endl;

    if((signed)mesh->mMaterialIndex >= 0)
    {
        aiMaterial* _material = scene->mMaterials[mesh->mMaterialIndex];

        // std::cout << "text = " << node->GetTextureCount() << std::endl;

        // 1. Diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(_material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. Specular maps
        vector<Texture> specularMaps = loadMaterialTextures(_material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // std::cout << "size 1 = " << diffuseMaps.size() << std::endl;
        // std::cout << "size 2 = " << specularMaps.size() << std::endl;
    }
    // std::cout << "size 3 = " << textures.size() << std::endl;   
    if (textures.size()!=0)
        return;
    Mesh tempMesh=Mesh(vertices, indices, textures);
    if((signed)mesh->mMaterialIndex >= 0)
    {
        tempMesh.material=new Material(scene->mMaterials[mesh->mMaterialIndex]);
        // tempMesh.material->print();
    }
    tempMesh.meshName=node->mName.C_Str();
    
    // printf("%f\t%f\t%f\t%f\n",t.a1,t.a2,t.a3,t.a4);
    // printf("%f\t%f\t%f\t%f\n",t.b1,t.b2,t.b3,t.b4);
    // printf("%f\t%f\t%f\t%f\n",t.c1,t.c2,t.c3,t.c4);
    // printf("%f\t%f\t%f\t%f\n",t.d1,t.d2,t.d3,t.d4);

    // std::cout << std::endl;
    meshes.push_back(tempMesh);
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
    // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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