#include <mesh.h>
#include <iostream>

Mesh::Mesh(vector<Vertex> vertices,vector<GLuint> indices,vector<Texture> textures)
:vertices(vertices),indices(indices),textures(textures),transformation(1.0f)
{
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), 
		&(vertices[0]), GL_STATIC_DRAW);

	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), 
		&(indices[0]), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    cmin=vec3(INFINITY,INFINITY,INFINITY);
	cmax=vec3(-INFINITY,-INFINITY,-INFINITY);
    for(Vertex &v: vertices)
    {
    	cmin.x=fmin(v.Position.x,cmin.x);
    	cmin.y=fmin(v.Position.y,cmin.y);
    	cmin.z=fmin(v.Position.z,cmin.z);

    	cmax.x=fmax(v.Position.x,cmax.x);
    	cmax.y=fmax(v.Position.y,cmax.y);
    	cmax.z=fmax(v.Position.z,cmax.z);
    }
    center=(cmin+cmax)/2.0f;

}

void Mesh::draw(Shader *shader)
{

	GLuint program=shader->getProgram();
	
	glBindBuffer(GL_ARRAY_BUFFER,vertexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementBufferObject);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "Model2")
		, 1, GL_FALSE, &transformation[0][0]);

	// Vertex Positions
	GLuint position=glGetAttribLocation(program,"position");
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
	// Vertex Color
	GLuint color=glGetAttribLocation(program,"color");
	glEnableVertexAttribArray(color);
	glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Color));
	// Vertex Tangent
	GLuint tangent=glGetAttribLocation(program,"tangent");
	glEnableVertexAttribArray(tangent);
	glVertexAttribPointer(tangent, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Tangent));

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

	material->loadToShader(program);
	glUniform1i(glGetUniformLocation(program,"useTexture"), textures.size());

	glBindVertexArray(vertexBufferObject);
	glDrawElements(GL_TRIANGLES, indices.size(),GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(position);
	glDisableVertexAttribArray(normal);
	glDisableVertexAttribArray(texCoords);
	glDisableVertexAttribArray(color);
	glDisableVertexAttribArray(tangent);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

	for (GLuint i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1,&vertexBufferObject);
	glDeleteVertexArrays(1,&elementBufferObject);
}