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
#include <ctime>

#include <vertex.h>
#include <texture.h>
#include <model.h>
#include <shader.h>
#include <mesh.h>
#include <car.h>
#include <scenario.h>

const std::string baseDir="../";
using namespace glm;

// FUNCTIONS
void clear(float,float,float,float);
void displayWin();
void reshapeWin(int,int);
void keyBorardFunc(unsigned char,int,int);
void idle();
void init();
// GLOBAL VARIABLES
std::map<std::string,Shader*> shaders;
std::map<std::string,Car*> cars;
Scenario *scenario;
float aspect=1;
float ang=0;

void displayWin()
{
	clear(0.22,0.22,0.22,1.0);
	shaders["car"]->reloadShader();
	shaders["scenario"]->reloadShader();

	vec3 eyePos=vec3(0,5,10);
	glm::mat4 Projection = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
	glm::mat4 View       = glm::lookAt(
	    eyePos,				// Camera is at (,,), in World Space
	    glm::vec3(0,0,0), 	// and looks at the origin
	    glm::vec3(0,1,0)  	// Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model;

	glm::vec4 Lpos(0.0f,2.0f,10.0f,0.0f);
	glm::vec4 ambient(0.6f,0.6f,0.6f,1.0f);
	glm::vec4 diffuse(0.3f,0.3f,0.3f,1.0f);
	glm::vec4 specular(0.7f,0.7f,0.7f,1.0f);

	//=======================================================//

	shaders["car"]->use(true);
	GLuint program=shaders["car"]->getProgram();

	glUniform4fv(glGetUniformLocation(program, "lightS.position"), 1, &Lpos[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.ambient"), 1, &ambient[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.diffuse"), 1, &diffuse[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.specular"), 1, &specular[0]);
	glUniform3fv(glGetUniformLocation(program, "eyePos"), 1, &eyePos[0]);

	Model = cars["car1"]->modelMatrix;
	// Model = glm::translate(Model,glm::vec3(0.0,0.0,ang)); 
	Model = glm::rotate(Model,ang,glm::vec3(0.0,1.0,0.0));
	ang+=0.003;

	glUniformMatrix4fv(glGetUniformLocation(program, "Projection")
		, 1, GL_FALSE, &Projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "View")
		, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "Model")
		, 1, GL_FALSE, &Model[0][0]);

    cars["car1"]->draw(shaders["car"]);

    //=========================================//

    shaders["scenario"]->use(true);
	program=shaders["scenario"]->getProgram();

	glUniform4fv(glGetUniformLocation(program, "lightS.position"), 1, &Lpos[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.ambient"), 1, &ambient[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.diffuse"), 1, &diffuse[0]);
	glUniform4fv(glGetUniformLocation(program, "lightS.specular"), 1, &specular[0]);
	glUniform3fv(glGetUniformLocation(program, "eyePos"), 1, &eyePos[0]);

	Model = scenario->modelMatrix;
	Model = glm::rotate(Model,ang,glm::vec3(0.0,1.0,0.0));
	// ang+=0.003;

	glUniformMatrix4fv(glGetUniformLocation(program, "Projection")
		, 1, GL_FALSE, &Projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "View")
		, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "Model")
		, 1, GL_FALSE, &Model[0][0]);

    scenario->draw(shaders["scenario"]);

    //=========================================//

    static float lastTime=std::clock();
    static int frameN=0;
    frameN++;
    if (frameN%50==0)
    {

    	float temp=(std::clock()-lastTime)/50.0/(double)CLOCKS_PER_SEC*1000;
    	std::cout << "Time per Frame = " << temp << std::endl;
    	lastTime=std::clock();
    }

	glutSwapBuffers();
}

void init()
{
	// Shader for all the cars
	shaders["car"]=new Shader(baseDir+"shaders/car.vs", baseDir+"shaders/car.frag");
	// Shader for the scenario
	shaders["scenario"]=new Shader(baseDir+"shaders/scenario.vs", baseDir+"shaders/scenario.frag");

	// Load the scenario mesh
	scenario=new Scenario(baseDir+"3dModels/city3/city.obj");
	scenario->modelMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(1.0f)); 

	// Load one car mesh
    cars["car1"]=new Car(baseDir+"3dModels/car9/car.obj");
    cars["car1"]->setColor(vec4(0.67,0.48,0.0,1.0));
    cars["car1"]->modelMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(2.0f)); 
    cars["car1"]->modelMatrix = glm::rotate((cars["car1"]->modelMatrix),80.0f,glm::vec3(0.0f,1.0f,0.0f));


    for (auto& m: cars)
    {
    	std::cout << "Mesh: \t";
    	for(auto& mesh: m.second->model.meshes)
    		std::cout << mesh.meshName << " , ";
    	std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
	int wWidth=720;
	int wHeight=480;
	aspect=(float)wWidth/(float)wHeight;
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
	aspect=(float)wWidth/(float)wHeight;
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