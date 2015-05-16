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
	void draw(Shader *shader);
// private:
	Model model;
	mat4 modelMatrix;
};

#endif