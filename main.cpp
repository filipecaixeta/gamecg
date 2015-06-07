#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <glm/glm.hpp>
#include <ctime>
#include <queue>

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
#include <hud.h>

#include <vehiclePhysic.h>
#include <SFX.h>
#include <music.h>
#include <chunk.h>

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
void changeMusic();
// GLOBAL VARIABLES
Light light(vec3(0.0f,2.0f,10.0f));
Camera *camera;

vector<int> carsList={9};
string scenarioName="3dModels/city14/city.obj";

VehiclePhysic* MainCar;
//Sounds
Music music;
Chunk changeStation;
int songNumber = 1;

std::map<std::string,Shader*> shaders;
std::map<std::string,Car*> cars;
Scenario *scenario;
float ang=0;
int gkey=0;
int speedKey=0;
int directionKey=0;
int brakeKey = 0;
int cameraMode=1;
float frontWheelAng=0.0;
float frontWheelAngMax=1.0;
std::queue<vec3> eyePosQ;
std::queue<vec3> lookAtPosQ;
bool blending=true;
bool stopCamera=false;
vec2 camera1Rotation=vec2(0.0f,0.0f);
vec2 mouselast=vec2(0,0);
Hud *hud;

void renderCar(int carNr)
{
	shaders["car"]->use(true);
	GLuint program=shaders["car"]->getProgram();
	
	char key[50];
	sprintf(key,"car%d",carNr);

	cars[key]->updateFrontWheel(frontWheelAng);
	static float speed=0.0;
	speed+=MainCar->m_vehicle->getCurrentSpeedKmHour()/200.0;
	if (speed>M_PI)
		speed-=M_PI;
	cars[key]->spinWheel(speed);

	mat4 Model = cars[key]->modelMatrix;
	mat4 ModelCamera = MainCar->GetVehicleMatrix()*glm::rotate(mat4(1.0f),ang,glm::vec3(0.0,1.0,0.0));
	Model = MainCar->GetVehicleMatrix() * Model;
	vec4 v;
	blending=true;
	if (cameraMode==1)
	{
		v=Model*vec4(0.35f,1.55,-0.5f,1.0f);
		camera->eyePos=vec3(v.x,v.y,v.z);

		mat4 rot;
		rot=glm::translate(mat4(1.0f),vec3(0.35f,1.55,-0.5f));
		rot=glm::rotate(rot,camera1Rotation.x,vec3(0.0f,1.0f,0.0f));
		rot=glm::rotate(rot,camera1Rotation.y,vec3(1.0f,0.0f,0.0f));
		rot=glm::translate(rot,-vec3(0.35f,1.55,-0.5f));

		v=Model*rot*vec4(0.35f,1.55,-0.1f,1.0f);
		camera->lookAtPos=vec3(v.x,v.y,v.z);
		music.changeVolume(110);
		blending=false;
	}
	if (cameraMode==2)
	{
		v=Model*vec4(0.0f,3.0,-5.0f,1.0f);
		eyePosQ.push(vec3(v.x,v.y,v.z));
		camera->eyePos=eyePosQ.front();
		if (eyePosQ.size()>=15)
			eyePosQ.pop();
		v=Model*vec4(0.0f,1.0f,6.0f,1.0f);
		lookAtPosQ.push(vec3(v.x,v.y,v.z));
		camera->lookAtPos=lookAtPosQ.front();
		if (lookAtPosQ.size()>=15)
			lookAtPosQ.pop();
		music.changeVolume(40);
	}
	if (cameraMode==3)
	{
		v=ModelCamera*vec4(7*sin(ang),4.0,7*cos(ang),1.0f);
		camera->eyePos=vec3(v.x,v.y,v.z);

		v=ModelCamera*vec4(0.0f,0.0f,0.0f,1.0f);
		camera->lookAtPos=vec3(v.x,v.y,v.z);
		music.changeVolume(40);
	}
	else
		ang=0;

	camera->update();

	light.loadToShader(program);
	camera->loadToShader(program,Model);

    cars[key]->draw(shaders["car"],blending);
}
void renderScene()
{
	
	shaders["scenario"]->use(true);
	GLuint program=shaders["scenario"]->getProgram();

	mat4 Model = scenario->modelMatrix;
	// Model = glm::rotate(Model,ang,glm::vec3(0.0,1.0,0.0));

	light.loadToShader(program);
	camera->loadToShader(program,Model);

    scenario->draw(shaders["scenario"],false);
}
void displayWin()
{
	clear(0.6,0.8,1.0,1.0);
	shaders["car"]->reloadShader();
	shaders["scenario"]->reloadShader();
	shaders["hud"]->reloadShader();

	if (speedKey==GLUT_KEY_UP)
	{
		MainCar->KeyDown(KEY_FORWARD);
	}
	else if (speedKey==GLUT_KEY_DOWN)
	{
		MainCar->KeyDown(KEY_BACK);	
	}
	else if (speedKey == 0)
	{
		MainCar->KeyUp(KEY_FORWARD);
	}

	if (directionKey==GLUT_KEY_RIGHT)
	{
		MainCar->KeyDown(KEY_RIGHT);
		frontWheelAng-=0.02;
		if (frontWheelAng<-frontWheelAngMax)
			frontWheelAng=-frontWheelAngMax;
	}
	else if (directionKey==GLUT_KEY_LEFT)
	{
		MainCar->KeyDown(KEY_LEFT);
		frontWheelAng+=0.02;
		if (frontWheelAng>frontWheelAngMax)
			frontWheelAng=frontWheelAngMax;
	}
	else if (directionKey == 0)
	{
		MainCar->KeyUp(KEY_LEFT);
		if (frontWheelAng>0.0)
			frontWheelAng-=0.06;
		else if (frontWheelAng<0.0)
			frontWheelAng+=0.06;
		if (abs(frontWheelAng)<0.06)
			frontWheelAng=0.0;
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

	shaders["hud"]->use(true);
	hud->setSpeed(MainCar->m_vehicle->getCurrentSpeedKmHour());
	hud->aspect=camera->aspect;

	hud->draw(shaders["hud"]);
	renderScene();
	for (int carNr: carsList)
	{
		renderCar(carNr);
	}


    static float lastTime=std::clock();
    static int frameN=0;
    frameN++;
    if (frameN%50==0)
    {

    	float temp=(std::clock()-lastTime)/50.0/(double)CLOCKS_PER_SEC*1000;
    	//std::cout << "Time per Frame = " << temp << std::endl;
    	lastTime=std::clock();
    }
    if (!stopCamera)
    	ang+=0.003;
	glutSwapBuffers();
}

void init()
{
	// Shader for all the cars
	shaders["car"]		= new Shader(baseDir+"shaders/car.vs", baseDir+"shaders/car.frag");
	// Shader for the scenario
	shaders["scenario"]	= new Shader(baseDir+"shaders/scenario.vs", baseDir+"shaders/scenario.frag");

	shaders["hud"]	= new Shader(baseDir+"shaders/hud.vs", baseDir+"shaders/hud.frag");

	// Load the scenario mesh
	scenario = new Scenario(baseDir+scenarioName);


	
	// Load one car mesh
	for (int carNr: carsList)
	{
		loadCar(carNr);
	}

	hud = new Hud(baseDir+"3dModels/hud/speedometer.png");

	char key[50];
	sprintf(key,"car%d",carsList[0]);

	MainCar = new VehiclePhysic(scenario,cars[key]);

	music.loadFile(baseDir+"sound/music/mus1.wav");
	music.playSound();
	music.changeVolume(40);
	music.pauseSound();
	changeStation.loadFile(baseDir+"sound/music/change.wav");
	changeStation.changeVolume(1, 127);
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
		case 'v':
			stopCamera=!stopCamera;
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
		case 'r':
			MainCar->ResetVehicle();
			break;
		case ' ':
			brakeKey = 1;
			break;
		case 'm':
			changeMusic();
			break;
		case 'p':
			if(music.isPlaying())
				music.pauseSound();
			else
				music.playSound();
			break;
	}
	if (key=='o')
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
void motionFunc(int x, int y)
{
	vec2 current=vec2(x,y);
	vec2 temp=(current-mouselast)*0.01f;
	camera1Rotation.x+=temp.x;
	camera1Rotation.y+=temp.y;
	if (camera1Rotation.y<-0.35)
		camera1Rotation.y=-0.35;
	if (camera1Rotation.y>0.7)
		camera1Rotation.y=0.7;
	mouselast=current;
}

void processMouse(int button, int state, int x, int y)
{
	mouselast=vec2(x,y);
	if (state==GLUT_UP)
		camera1Rotation=vec2(0.0f,0.0f);
}

int main(int argc, char *argv[])
{
	int wWidth=720;
	int wHeight=480;

	const char windowName[]="Racing Game";
	glutInit(&argc, argv);
    glutInitWindowSize(wWidth,wHeight);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    // glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH  | GLUT_MULTISAMPLE );
 	glutCreateWindow(windowName);
 	glutSetCursor(GLUT_CURSOR_NONE);
 	glLineWidth(5);

 	glEnable(GL_DEPTH_TEST); 
 	glEnable ( GL_ALPHA_TEST );

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
	glutMotionFunc(motionFunc);
	glutMouseFunc(processMouse);
    glutMainLoop();
    return 0;
}

void loadCar(int carNr)
{
	char path[50];
	char key[50];
	sprintf(path,"%s/3dModels/car%d/car.obj",baseDir.c_str(),carNr);
	sprintf(key,"car%d",carNr);
	cars[key] = new Car(path);
	// cars[key]->setColor(vec4(0.02,0.02,0.02,1.0));
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

void changeMusic()
{
	songNumber++;
	if(songNumber == 3)
		songNumber = 1;
	music.stopSound();
	changeStation.playSound(ONCE, 1);
	std::string tmp = std::to_string(songNumber);
	music.loadFile(baseDir+"sound/music/mus"+tmp+".wav");
	music.playSound();

}