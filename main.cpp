#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vertex.h>
#include <texture.h>
#include <model.h>
#include <shader.h>
#include <mesh.h>

const std::string baseDir="../";

// FUNCTIONS
void clear(float,float,float,float);
void displayWin();
void reshapeWin(int,int);
void keyBorardFunc(unsigned char,int,int);
void idle();
void init();
// GLOBAL VARIABLES
std::map<std::string,Shader*> shaders;
std::map<std::string,Model*> models;
float ang=0;

void displayWin()
{
	clear(0.0,0.0,0.0,1.0);
	shaders["camaro"]->reloadShader();
	shaders["camaro"]->use(true);

	glm::mat4 Projection = glm::perspective(45.0f, 720.0f/480.0f, 0.1f, 100.0f);
	glm::mat4 View       = glm::lookAt(
	    glm::vec3(0,5,10), // Camera is at (,,), in World Space
	    glm::vec3(0,0,0), // and looks at the origin
	    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::scale(glm::mat4(1.0f),glm::vec3(2.0f)); 
	// Model = glm::translate(Model,glm::vec3(0,-10,0));
	// Model = glm::rotate(Model,90.0f,glm::vec3(1.0,0.0,0.0));
	Model = glm::rotate(Model,ang,glm::vec3(0.0,1.0,0.0));
	ang+=0.01;

	GLuint program=shaders["camaro"]->getProgram();

	glUniformMatrix4fv(glGetUniformLocation(program, "Projection")
		, 1, GL_FALSE, &Projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "View")
		, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "Model")
		, 1, GL_FALSE, &Model[0][0]);

	glm::vec4 Lpos(0.0f,5.0f,10.0f,0.0f);
	glm::vec4 ambient(0.3f,0.3f,0.3f,1.0f);
	glm::vec4 diffuse(0.8f,0.8f,0.8f,1.0f);
	glm::vec4 specular(0.9f,0.9f,0.9f,1.0f);

	glUniform4fv(glGetUniformLocation(program, "lightS.position"), 1, &Lpos[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.ambient"), 1, &ambient[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.diffuse"), 1, &diffuse[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.specular"), 1, &specular[0]);

    models["camaro"]->draw(shaders["camaro"]);

	glutSwapBuffers();
}

void init()
{
	shaders["camaro"]=new Shader(baseDir+"shaders/shader.vs", baseDir+"shaders/shader.frag");
    models["camaro"]=new Model(baseDir+"3dModels/camaro/CAMARO.obj");

    for (auto& m: models)
    {
    	std::cout << "Mesh: \t";
    	for(auto& mesh: m.second->meshes)
    		std::cout << mesh.meshName << " , ";
    	std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
	int wWidth=720;
	int wHeight=480;
	const char windowName[]="window";
	glutInit(&argc, argv);
    glutInitWindowSize(wWidth,wHeight);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
 	glutCreateWindow(windowName);
 	
 	glEnable(GL_DEPTH_TEST); 
 	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
    	std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    	return 1;
    }
    init();
    glutDisplayFunc(displayWin);
	glutReshapeFunc(reshapeWin);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyBorardFunc);
    glutMainLoop();

return 0;
}

void clear(float r,float g, float b, float a)
{
	glClearColor(r,g,b,a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void reshapeWin(int wWidth, int wHeight) 
{
	glViewport( 0, 0, wWidth, wHeight );
	glutPostRedisplay();
}

void keyBorardFunc(unsigned char key, int, int)
{
	switch(key)
	{
		case 27:
		case 'q':
			glutLeaveMainLoop();
			break;
	}
	glutPostRedisplay();
}

void idle()
{
	glutPostRedisplay();
}