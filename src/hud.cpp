#include <hud.h>
#include <stb_image.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Hud::Hud(string imageName)
{

	textureId=TextureFromFile(imageName);

	verticesPlane.push_back(vec3(-1.0,1.0,0.9991));
	verticesPlane.push_back(vec3(-1.0,-1.0,0.9991));
	verticesPlane.push_back(vec3(1.0,-1.0,0.99991));
	verticesPlane.push_back(vec3(1.0,1.0,0.99991));

	verticesLine.push_back(vec3(0.0,0.0,0.9999));
	verticesLine.push_back(vec3(0.0,1.0,0.9999));

	glGenBuffers(1, &vertexBufferObjectPlane);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectPlane);
	glBufferData(GL_ARRAY_BUFFER, verticesPlane.size()*sizeof(vec3), 
		&(verticesPlane[0]), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vertexBufferObjectLine);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectLine);
	glBufferData(GL_ARRAY_BUFFER, verticesLine.size()*sizeof(vec3), 
		&(verticesLine[0]), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Hud::draw(Shader *shader)
{
	GLuint program=shader->getProgram();
	
	glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);

	mat4 Model;
	
	Model[2][2]=-Model[2][2];
	Model=glm::scale(Model,vec3(0.25,0.25,1.0));

	if (aspect>=1)
		Model[0][0]/=aspect;
	else
		Model[1][1]*=aspect;

	color=vec3(1.0,0.0,0.0f);

	glBindBuffer(GL_ARRAY_BUFFER,vertexBufferObjectPlane);
	// Vertex Positions
	GLuint position=glGetAttribLocation(program,"position");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),(GLvoid*)0);
	
    glUniformMatrix4fv(glGetUniformLocation(program, "Model")
		, 1, GL_FALSE, &Model[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glUniform1f(glGetUniformLocation(program,"texture1"), 0);
	glUniform1i(glGetUniformLocation(program,"useTexture"), 1);

	glBindVertexArray(vertexBufferObjectPlane);
	glDrawArrays(GL_QUADS,0, 4);

	glDisableVertexAttribArray(position);
	glBindBuffer(GL_ARRAY_BUFFER,0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,0);

    // ==========================

    glBindBuffer(GL_ARRAY_BUFFER,vertexBufferObjectLine);
    // Vertex Positions
	position=glGetAttribLocation(program,"position");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),(GLvoid*)0);

	glUniform1i(glGetUniformLocation(program,"useTexture"), 0);
	glUniform3fv(glGetUniformLocation(program,"color"),1, &(color[0]));

	speed=(float)speed*M_PI/180.0f;
	Model=glm::rotate(Model,-speed,vec3(0.0f,0.0f,1.0f));

	glUniformMatrix4fv(glGetUniformLocation(program, "Model")
		, 1, GL_FALSE, &Model[0][0]);

	glBindVertexArray(vertexBufferObjectLine);
	glDrawArrays(GL_LINES,0, verticesLine.size());

	glDisableVertexAttribArray(position);
	glBindBuffer(GL_ARRAY_BUFFER,0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,0);
}

void Hud::setSpeed(float _speed)
{
	_speed*=0.7;
	if (_speed<0)
		_speed=0;
	if (_speed>280)
		_speed=280;
	this->speed=_speed-132;
}

GLint Hud::TextureFromFile(string filename)
{
    //Generate texture ID and load texture data 
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

Hud::~Hud()
{

}