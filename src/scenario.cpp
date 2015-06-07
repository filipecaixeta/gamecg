#include <scenario.h>

Scenario::Scenario(string path):model(path)
{

}
Scenario::~Scenario()
{

}
void Scenario::draw(Shader *shader,bool blending)
{
	model.draw(shader,blending);
}