#include <mesh.h>

Mesh::Mesh(vector<Vertex> vertices,vector<GLuint> indices,vector<Texture> textures)
:vertices(vertices),indices(indices),textures(textures)
{
	glGenVertexArrays(1,&vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), 
		&(vertices[0]), GL_STATIC_DRAW);

	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), 
		&(indices[0]), GL_STATIC_DRAW);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::draw(Shader *shader)
{
	shader->use(true);
	GLuint program=shader->getProgram();
	// Vertex Positions
	GLuint position=glGetAttribLocation(program,"position");
	glBindBuffer(GL_ARRAY_BUFFER,vertexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementBufferObject);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)0);
    // Vertex Normals
    GLuint normal=glGetAttribLocation(program,"normal");
	glEnableVertexAttribArray(normal);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Normal));
	// Vertex Texture Coords
	GLuint texCoords=glGetAttribLocation(program,"texCoords");
	glEnableVertexAttribArray(texCoords);
	glVertexAttribPointer(texCoords, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, TexCoords));

	GLuint diffuseNro=1;
	GLuint specularNro=1;
	int i=0;
	for (Texture& texture : textures)
	{
		glActiveTexture(GL_TEXTURE0+i);
		char name[27];
		if (texture.type == TYPE_DIFFUSE)
			sprintf(name,"material.texture_difuse%d",diffuseNro++);
		else if (texture.type == TYPE_SPECULAR)
			sprintf(name,"material.texture_specular%d",specularNro++);

		glUniform1f(glGetUniformLocation(program,name), i);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		i++;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vertexArrayObject);
	glDrawElements(GL_TRIANGLES, indices.size(),GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(position);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

	for (GLuint i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    shader->use(false);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1,&vertexBufferObject);
	glDeleteVertexArrays(1,&elementBufferObject);
}