#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <glm/glm.hpp>
#include <ctime>

#include <vertex.h>
#include <texture.h>
#include <model.h>
#include <shader.h>
#include <mesh.h>
#include <car.h>
#include <scenario.h>
#include <kdtree.hpp>
#include <light.h>
#include <camera.h>
#include <queue>
#include <vehiclePhysic.h>


//TEST*************************************
// #define DEBUG_MODE
#ifdef DEBUG_MODE
#include "GlutStuff.h"
#include "GLDebugDrawer.h"
#include "btBulletDynamicsCommon.h"

GLDebugDrawer	gDebugDrawer;
#endif
//****************************************

#define RANDF (rand()%100)*0.01

const std::string baseDir="../";
using namespace glm;

// FUNCTIONS
void clear(float,float,float,float);
void displayWin();
void reshapeWin(int,int);
void keyBorardFunc(unsigned char,int,int);
void idle();
void init();
void loadCar(int carNr);
// GLOBAL VARIABLES
Light light(vec3(0.0f,2.0f,10.0f));
Camera *camera;

VehiclePhysic* MainCar;

std::map<std::string,Shader*> shaders;
std::map<std::string,Car*> cars;
Scenario *scenario;
float ang=0;
vector<int> carsList={9};
mat4 moveCar(1.0f);
int gkey=0;
int speedKey=0;
int directionKey=0;
int brakeKey = 0;
int cameraMode=1;
float carang=0.0;
float carang2=0.0;
int sentido=1;
std::queue<vec3> eyePosQ;
std::queue<vec3> lookAtPosQ;

void renderCar(int carNr)
{
	shaders["car"]->use(true);
	GLuint program=shaders["car"]->getProgram();
	
	char key[50];
	sprintf(key,"car%d",carNr);

	cars[key]->updateFrontWheel(carang2);
	cars[key]->spinWheel(carang);
	carang+=0.5;
	carang2+=0.01*sentido;
	if (fabs(carang2)>1.0)
	{
		if (sentido==1)
			sentido=-1;
		else
			sentido=1;
	}

	mat4 Model = cars[key]->modelMatrix;
	Model = glm::rotate(mat4(1.0f),ang,glm::vec3(0.0,1.0,0.0))*moveCar;

	Model = MainCar->GetVehicleMatrix() * Model;
	vec4 v;
	
	if (cameraMode==1)
	{
		if (string(key)=="car2")
			v=Model*vec4(0.35f,1.016,0.3f,1.0f);
		else if (string(key)=="car9")
			v=Model*vec4(0.35f,1.55,-0.5f,1.0f);
		camera->eyePos=vec3(v.x,v.y,v.z);
		v=Model*vec4(0.0f,1.0f,6.0f,1.0f);
		camera->lookAtPos=vec3(v.x,v.y,v.z);
	}
	if (cameraMode==2)
	{
		v=Model*vec4(0.0f,3.0,-10.0f,1.0f);
		eyePosQ.push(vec3(v.x,v.y,v.z));
		camera->eyePos=eyePosQ.front();
		if (eyePosQ.size()>=15)
			eyePosQ.pop();
		v=Model*vec4(0.0f,1.0f,6.0f,1.0f);
		lookAtPosQ.push(vec3(v.x,v.y,v.z));
		camera->lookAtPos=lookAtPosQ.front();
		if (lookAtPosQ.size()>=15)
			lookAtPosQ.pop();
	}
	if (cameraMode==3)
	{
		// ang=1;
		// v=Model*vec4(4*sin(ang),1.5,3*cos(ang),1.0f);
		v=Model*vec4(5*sin(ang),4.0,5*cos(ang),1.0f);
		camera->eyePos=vec3(v.x,v.y,v.z);

		v=Model*vec4(0.0f,0.0f,0.0f,1.0f);
		camera->lookAtPos=vec3(v.x,v.y,v.z);
	}
	else
		ang=0;

	camera->update();

	light.loadToShader(program);
	camera->loadToShader(program,Model);

    cars[key]->draw(shaders["car"]);
}
void renderScene()
{
	
	shaders["scenario"]->use(true);
	GLuint program=shaders["scenario"]->getProgram();

	mat4 Model = scenario->modelMatrix;
	// Model = glm::rotate(Model,ang,glm::vec3(0.0,1.0,0.0));

	light.loadToShader(program);
	camera->loadToShader(program,Model);

    scenario->draw(shaders["scenario"]);
}
void displayWin()
{
	clear(0.6,0.8,1.0,1.0);
	shaders["car"]->reloadShader();
	shaders["scenario"]->reloadShader();

	if (speedKey==GLUT_KEY_UP)
	{
		MainCar->KeyDown(KEY_FORWARD);

		if (directionKey==GLUT_KEY_LEFT)
			MainCar->KeyDown(KEY_LEFT);
		else if (directionKey==GLUT_KEY_RIGHT)
			MainCar->KeyDown(KEY_RIGHT);
		else if (directionKey == 0)
			MainCar->KeyUp(KEY_LEFT);
	}
	else if (speedKey==GLUT_KEY_DOWN)
	{
		MainCar->KeyDown(KEY_BACK);
		
		if (directionKey==GLUT_KEY_RIGHT)
			MainCar->KeyDown(KEY_RIGHT);
		else if (directionKey==GLUT_KEY_LEFT)
			MainCar->KeyDown(KEY_LEFT);
		else if (directionKey == 0)
			MainCar->KeyUp(KEY_LEFT);
	}
	else if (speedKey == 0)
	{
		MainCar->KeyUp(KEY_FORWARD);

		if (directionKey==GLUT_KEY_RIGHT)
			MainCar->KeyDown(KEY_LEFT);
		else if (directionKey==GLUT_KEY_LEFT)
			MainCar->KeyDown(KEY_RIGHT);
		else if (directionKey == 0)
			MainCar->KeyUp(KEY_LEFT);
	}

	if(brakeKey)
	{
		MainCar->KeyDown(KEY_SPACE);
	}
	else
	{
		MainCar->KeyUp(KEY_SPACE);
	}

	MainCar->MoveVehicle();

	for (int carNr: carsList)
	{
		renderCar(carNr);
	}

	renderScene();

    static float lastTime=std::clock();
    static int frameN=0;
    frameN++;
    if (frameN%50==0)
    {

    	float temp=(std::clock()-lastTime)/50.0/(double)CLOCKS_PER_SEC*1000;
    	std::cout << "Time per Frame = " << temp << std::endl;
    	lastTime=std::clock();
    }

    ang+=0.003;
	glutSwapBuffers();
}
void init()
{
	// Shader for all the cars
	shaders["car"]		= new Shader(baseDir+"shaders/car.vs", baseDir+"shaders/car.frag");
	// Shader for the scenario
	shaders["scenario"]	= new Shader(baseDir+"shaders/scenario.vs", baseDir+"shaders/scenario.frag");

	// Load the scenario mesh
	scenario = new Scenario(baseDir+"3dModels/city7/city.obj");
	std::cerr << "SHADERS" << std::endl;

	// Load one car mesh
	for (int carNr: carsList)
	{
		loadCar(carNr);
	}

	char key[50];
	sprintf(key,"car%d",carsList[0]);

	MainCar = new VehiclePhysic(scenario,cars[key]);
}
void processSpecialKeys(int key, int , int ) 
{
	gkey=key;
	switch( key ) 
	{
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN:
			speedKey=key;
			break;
		case GLUT_KEY_RIGHT:
		case GLUT_KEY_LEFT:
			directionKey=key;
			break;
	}
}
void keyBoardUp( int key, int x, int y )
{
	switch( key ) 
	{
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN:
			speedKey=0;
			break;
		case GLUT_KEY_RIGHT:
		case GLUT_KEY_LEFT:
			directionKey=0;
			break;
	}
	gkey=0;
}
void keyBoardFunc(unsigned char key, int, int)
{
	// std::cout << key << std::endl;
	switch(key)
	{
		case 27:
		case 'q':
			glutLeaveMainLoop();
			break;
		case 'c':
			cameraMode++;
			if (cameraMode==4)
				cameraMode=1;
			break;
		case 'w':
			speedKey=GLUT_KEY_UP;
			break;
		case 's':
			speedKey=GLUT_KEY_DOWN;
			break;
		case 'd':
			directionKey=GLUT_KEY_RIGHT;
			break;
		case 'a':
			directionKey=GLUT_KEY_LEFT;
			break;
		case 'p':
			directionKey=GLUT_KEY_LEFT;
			break;
		case 'r':
			MainCar->ResetVehicle();
			break;
		case ' ':
			brakeKey = 1;
			break;
	}
	if (key=='p')
	{
		for (auto& c: cars)
		{
			c.second->setColor(vec4(RANDF,RANDF,RANDF,1.0));
		}
	}
	glutPostRedisplay();
}
void keyBoardUpFunc(unsigned char key, int, int)
{
	switch(key)
	{
		case 'w':
		case 's':
			speedKey=0;
			break;
		case 'd':
		case 'a':
			directionKey=0;
			break;
		case ' ':
			brakeKey = 0;
			break;
	}
}
int main(int argc, char *argv[])
{
	
#ifndef DEBUG_MODE

	int wWidth=720;
	int wHeight=480;

	const char windowName[]="window";
	glutInit(&argc, argv);
    glutInitWindowSize(wWidth,wHeight);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
 	glutCreateWindow(windowName);
 	
 	glEnable(GL_DEPTH_TEST); 
 	glEnable ( GL_ALPHA_TEST ) ;
 	glAlphaFunc ( GL_GREATER, 0.8 ) ;
 	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

 	camera = new Camera(vec3(0.0f,5.0f,-10.0f),vec3(0.0,0.0,0.0),wWidth,wHeight);

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
	glutKeyboardFunc(keyBoardFunc);
	glutKeyboardUpFunc(keyBoardUpFunc);
	glutSpecialFunc(processSpecialKeys);
	glutSpecialUpFunc(keyBoardUp);
    glutMainLoop();
    return 0;


#else
    std::cerr << "LOL";
    int wWidth=720;
	int wHeight=480;

    camera = new Camera(vec3(0.0f,5.0f,-10.0f),vec3(0.0,0.0,0.0),wWidth,wHeight);
    //init();
    MainCar = new VehiclePhysic;
    MainCar->getDynamicsWorld()->setDebugDrawer(&gDebugDrawer);
	return glutmain(argc, argv,640,480,"Bullet Vehicle Demo. http://www.continuousphysics.com/Bullet/phpBB2/", MainCar);;
#endif
}

void loadCar(int carNr)
{
	char path[50];
	char key[50];
	sprintf(path,"%s/3dModels/car%d/car.obj",baseDir.c_str(),carNr);
	sprintf(key,"car%d",carNr);
	cars[key] = new Car(path);
}

void clear(float r,float g, float b, float a)
{
	glClearColor(r,g,b,a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void reshapeWin(int wWidth, int wHeight) 
{
	camera->setAspect(wWidth,wHeight);
	glViewport( 0, 0, wWidth, wHeight );
	glutPostRedisplay();
}

void idle()
{
	glutPostRedisplay();
}