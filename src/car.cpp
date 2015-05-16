#include <car.h>

Car::Car(string path):model(path)
{
	for(auto& mesh: model.meshes)
	{
		if (mesh.material->name.find("Glass")==0)
			mesh.material->color_transparent=vec4(1.0f,1.0f,1.0f,0.1f);
		else if (mesh.material->name.find("BreakLight")==0)
			mesh.material->color_emissive=vec4(1.0f,0.0f,0.0f,1.0f);
	}
}
void Car::updateFrontWheel(float ang)
{

}
void Car::spinWheel(float ang)
{

}
void Car::setColor(vec4 color)
{
	for(auto& mesh: model.meshes)
	{
		if (mesh.material->name.find("Body")==0)
			mesh.material->color_diffuse=color;
	}
}
void Car::updatePosition()
{

}
void Car::draw(Shader *shader)
{
	model.draw(shader);
}
Car::~Car()
{
	
}