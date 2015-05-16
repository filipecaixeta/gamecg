#include <scenario.h>

Scenario::Scenario(string path):model(path)
{

}
Scenario::~Scenario()
{

}
void Scenario::draw(Shader *shader)
{
	model.draw(shader);
}