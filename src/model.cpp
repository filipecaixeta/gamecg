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
         aiProcess_GenSmoothNormals );
	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR: ASSIMP " << importer.GetErrorString()<< std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);

    cmin=vec3(INFINITY,INFINITY,INFINITY);
    cmax=vec3(-INFINITY,-INFINITY,-INFINITY);
    for(Mesh &m: meshes)
    {
        cmin.x=fmin(m.cmin.x,cmin.x);
        cmin.y=fmin(m.cmin.y,cmin.y);
        cmin.z=fmin(m.cmin.z,cmin.z);

        cmax.x=fmax(m.cmax.x,cmax.x);
        cmax.y=fmax(m.cmax.y,cmax.y);
        cmax.z=fmax(m.cmax.z,cmax.z);
    }
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
        vec4 vecTemp4;

        vecTemp4.x = mesh->mVertices[i].x;
        vecTemp4.y = mesh->mVertices[i].y;
        vecTemp4.z = mesh->mVertices[i].z;
        vec4 temp=tr*vecTemp4;
        vertex.Position = vec3(temp.x,temp.y,temp.z);//vecTemp;

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

    if((signed)mesh->mMaterialIndex >= 0)
    {
        aiMaterial* _material = scene->mMaterials[mesh->mMaterialIndex];
        // 1. Diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(_material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. Specular maps
        vector<Texture> specularMaps = loadMaterialTextures(_material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    Mesh tempMesh=Mesh(vertices, indices, textures);
    tempMesh.meshName=node->mName.C_Str();
    tempMesh.transformation=tr;
    if((signed)mesh->mMaterialIndex >= 0)
    {
        tempMesh.material=new Material(scene->mMaterials[mesh->mMaterialIndex]);
    }

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}
Model::~Model()
{

}
void Model::draw(Shader *shader,bool blending)
{
    if (blending)
    {
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
    }
    else
    {
        glEnable (GL_ALPHA_TEST);
        glAlphaFunc (GL_GREATER, 0.8);
    }
    for (Mesh& mesh: meshes)
        if (mesh.material->name.find("GLASS_INTERIOR")!=0)
		  mesh.draw(shader);
    if (blending)
        glEnable(GL_BLEND);
    for (Mesh& mesh: meshes)
        if (mesh.material->name.find("GLASS_INTERIOR")==0)
            mesh.draw(shader);
}