#include <car.h>

Car::Car(string path):model(path)
{
	for(auto& mesh: model.meshes)
	{
		if (mesh.material->name.find("GLASS_INTERIOR")==0)
			mesh.material->color_transparent=vec4(1.0f,1.0f,1.0f,0.3f);
		else if (mesh.material->name.find("BreakLight")==0)
			mesh.material->color_emissive=vec4(1.0f,0.0f,0.0f,1.0f);
		else if (mesh.material->name.find("LIGHT_BACK")==0)
			mesh.material->color_emissive=vec4(1.0f,0.0f,0.0f,1.0f);
		else if (mesh.material->name.find("LIGHT_FRONT")==0)
			mesh.material->color_emissive=vec4(1.0f,1.0f,1.0f,1.0f);
		
		if (mesh.meshName.find("Tire_fl")==0)
			Tire_fl.push_back(&mesh);
		else if (mesh.meshName.find("Tire_fr")==0)
			Tire_fr.push_back(&mesh);
		else if (mesh.meshName.find("Tire_bl")==0)
			Tire_bl.push_back(&mesh);
		else if (mesh.meshName.find("Tire_br")==0)
			Tire_br.push_back(&mesh);
		else if (mesh.meshName.find("steeringwheel")==0)
			steeringwheel=&mesh;
		
	}
	vec3 cmin;
	vec3 cmax;
	cmin=vec3(INFINITY,INFINITY,INFINITY);
	cmax=vec3(-INFINITY,-INFINITY,-INFINITY);
	for(Mesh *m: Tire_fl)
    {
        cmin.x=fmin(m->cmin.x,cmin.x);
        cmin.y=fmin(m->cmin.y,cmin.y);
        cmin.z=fmin(m->cmin.z,cmin.z);

        cmax.x=fmax(m->cmax.x,cmax.x);
        cmax.y=fmax(m->cmax.y,cmax.y);
        cmax.z=fmax(m->cmax.z,cmax.z);
    }
    Tire_fl_center=(cmin+cmax)/2.0f;
    cmin=vec3(INFINITY,INFINITY,INFINITY);
	cmax=vec3(-INFINITY,-INFINITY,-INFINITY);
	for(Mesh *m: Tire_fr)
    {
        cmin.x=fmin(m->cmin.x,cmin.x);
        cmin.y=fmin(m->cmin.y,cmin.y);
        cmin.z=fmin(m->cmin.z,cmin.z);

        cmax.x=fmax(m->cmax.x,cmax.x);
        cmax.y=fmax(m->cmax.y,cmax.y);
        cmax.z=fmax(m->cmax.z,cmax.z);
    }
    Tire_fr_center=(cmin+cmax)/2.0f;
    cmin=vec3(INFINITY,INFINITY,INFINITY);
	cmax=vec3(-INFINITY,-INFINITY,-INFINITY);
	for(Mesh *m: Tire_bl)
    {
        cmin.x=fmin(m->cmin.x,cmin.x);
        cmin.y=fmin(m->cmin.y,cmin.y);
        cmin.z=fmin(m->cmin.z,cmin.z);

        cmax.x=fmax(m->cmax.x,cmax.x);
        cmax.y=fmax(m->cmax.y,cmax.y);
        cmax.z=fmax(m->cmax.z,cmax.z);
    }
    Tire_bl_center=(cmin+cmax)/2.0f;
    cmin=vec3(INFINITY,INFINITY,INFINITY);
	cmax=vec3(-INFINITY,-INFINITY,-INFINITY);
	for(Mesh *m: Tire_br)
    {
        cmin.x=fmin(m->cmin.x,cmin.x);
        cmin.y=fmin(m->cmin.y,cmin.y);
        cmin.z=fmin(m->cmin.z,cmin.z);

        cmax.x=fmax(m->cmax.x,cmax.x);
        cmax.y=fmax(m->cmax.y,cmax.y);
        cmax.z=fmax(m->cmax.z,cmax.z);
    }
    Tire_br_center=(cmin+cmax)/2.0f;
	modelMatrix = glm::mat4(1.0f);
}
void Car::updateFrontWheel(float ang)
{
	for(Mesh *m: Tire_fl)
	{
		m->transformation = glm::translate(mat4(1.0f),Tire_fl_center);
		m->transformation = glm::rotate(m->transformation,ang,glm::vec3(0.0f,1.0f,0.0f));
		m->transformation = glm::translate(m->transformation,-Tire_fl_center);
	}
	for(Mesh *m: Tire_fr)
	{
		m->transformation = glm::translate(mat4(1.0f),Tire_fr_center);
		m->transformation = glm::rotate(m->transformation,ang,glm::vec3(0.0f,1.0f,0.0f));
		m->transformation = glm::translate(m->transformation,-Tire_fr_center);
	}
	steeringwheel->transformation = glm::translate(mat4(1.0f),steeringwheel->center);
	steeringwheel->transformation = glm::rotate(steeringwheel->transformation,0.261799f,glm::vec3(1.0f,0.0f,0.0f));
	steeringwheel->transformation = glm::rotate(steeringwheel->transformation,-ang,glm::vec3(0.0f,0.0f,1.0f));
	steeringwheel->transformation = glm::rotate(steeringwheel->transformation,-0.261799f,glm::vec3(1.0f,0.0f,0.0f));
	steeringwheel->transformation = glm::translate(steeringwheel->transformation,-steeringwheel->center);
}
void Car::spinWheel(float ang)
{
	for(Mesh *m: Tire_fl)
	{
		m->transformation = glm::translate(m->transformation,Tire_fl_center);
		m->transformation = glm::rotate(m->transformation,ang,glm::vec3(1.0f,0.0f,0.0f));
		m->transformation = glm::translate(m->transformation,-Tire_fl_center);
	}
	for(Mesh *m: Tire_fr)
	{
		m->transformation = glm::translate(m->transformation,Tire_fr_center);
		m->transformation = glm::rotate(m->transformation,ang,glm::vec3(1.0f,0.0f,0.0f));
		m->transformation = glm::translate(m->transformation,-Tire_fr_center);
	}
	for(Mesh *m: Tire_bl)
	{
		m->transformation = glm::translate(mat4(1.0f),Tire_bl_center);
		m->transformation = glm::rotate(m->transformation,ang,glm::vec3(1.0f,0.0f,0.0f));
		m->transformation = glm::translate(m->transformation,-Tire_bl_center);
	}
	for(Mesh *m: Tire_br)
	{
		m->transformation = glm::translate(mat4(1.0f),Tire_br_center);
		m->transformation = glm::rotate(m->transformation,ang,glm::vec3(1.0f,0.0f,0.0f));
		m->transformation = glm::translate(m->transformation,-Tire_br_center);
	}
}
void Car::setColor(vec4 color)
{
	for(auto& mesh: model.meshes)
	{
		if (mesh.material->name.find("BODY")==0)
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