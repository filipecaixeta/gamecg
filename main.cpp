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

void displayWin()
{
	clear(0.0,0.0,0.0,1.0);
	shaders["nanosuit"]->reloadShader();

    models["nanosuit"]->draw(shaders["nanosuit"]);

	glutSwapBuffers();
}

void init()
{
	shaders["nanosuit"]=new Shader(baseDir+"shaders/shader.vs", baseDir+"shaders/shader.frag");
    models["nanosuit"]=new Model(baseDir+"3dModels/nanosuit/nanosuit.obj");
}

int main(int argc, char *argv[])
{
	int wWidth=720;
	int wHeight=480;
	const char windowName[]="window";
	glutInit(&argc, argv);
    glutInitWindowSize(wWidth,wHeight);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
 	glutCreateWindow(windowName);
 	
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