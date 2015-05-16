#ifndef	SCENARIO_H
#define SCENARIO_H

#include <string>
#include <vector>
#include <iostream>
#include <shader.h>
#include <model.h>
#include <glm/glm.hpp>
using std::string;
using std::vector;
using namespace glm;

class Scenario
{
public:
	Scenario(string path);
	~Scenario();
	void draw(Shader *shader);
	Model model;
	mat4 modelMatrix;
};

#endif