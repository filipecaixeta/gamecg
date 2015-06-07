#ifndef	CAR_H
#define CAR_H
#include <string>
#include <vector>
#include <iostream>
#include <shader.h>
#include <model.h>
#include <glm/glm.hpp>
using std::string;
using std::vector;
using namespace glm;

class Car
{
public:
	Car(string path);
	~Car();
	void updateFrontWheel(float ang);
	void spinWheel(float ang);
	void setColor(vec4 color);
	void updatePosition();
	void draw(Shader *shader,bool blending);
// private:
	Model model;
	mat4 modelMatrix;
	vector<Mesh*> Tire_fl;
	vec3 Tire_fl_center;
	vector<Mesh*> Tire_fr;
	vec3 Tire_fr_center;
	vector<Mesh*> Tire_bl;
	vec3 Tire_bl_center;
	vector<Mesh*> Tire_br;
	vec3 Tire_br_center;
	Mesh *steeringwheel;
};

#endif