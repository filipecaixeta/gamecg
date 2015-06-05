#ifndef VEHICLEPHYSIC_H
#define VEHICLEPHYSIC_H

class btVehicleTuning;
struct btVehicleRaycaster;
class btCollisionShape;

#include "BulletDynamics/Vehicle/btRaycastVehicle.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"

#include <vector>

#include <glm/glm.hpp>
#include <scenario.h>
#include <car.h>
#include <music.h>
#include <chunk.h>

#include "GlutDemoApplication.h"

enum Keys
{
	KEY_FORWARD,
	KEY_BACK,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_SPACE
};

typedef struct Wheel
{
	glm::vec3 pos;
	glm::vec3 rot;	
}Wheel;

typedef struct CarPos
{
	glm::vec3 chassisPos;
	glm::vec3 chassisRot;

	Wheel wheels[4];
}CarPos;


class VehiclePhysic : public GlutDemoApplication
{
public:
	btRigidBody* m_carChassis;

	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

	class btBroadphaseInterface*	m_overlappingPairCache;

	class btCollisionDispatcher*	m_dispatcher;

	class btConstraintSolver*	m_constraintSolver;

	class btDefaultCollisionConfiguration* m_collisionConfiguration;

	class btTriangleIndexVertexArray*	m_indexVertexArrays;

	btVector3*	m_vertices;

	
	btRaycastVehicle::btVehicleTuning	m_tuning;
	btVehicleRaycaster*	m_vehicleRayCaster;
	btRaycastVehicle*	m_vehicle;
	btCollisionShape*	m_wheelShape;

	VehiclePhysic();
	VehiclePhysic(Scenario *scene,Car *car);
	virtual ~VehiclePhysic();
	void initPhysics();
	void KeyDown(int key);
	void KeyUp(int key);
	void MoveVehicle();
	void ResetVehicle();
	void CreateTerrain(btTransform tr, btCollisionShape* groundShape);
	glm::mat4 GetVehicleMatrix();
	btDynamicsWorld* GetDynamicsWorld();

private:
	float	gEngineForce;
	float	gBreakingForce;

	float	maxEngineForce;//this should be engine/velocity dependent
	float	maxBreakingForce;

	float	gVehicleSteering;
	float	steeringIncrement;
	float	steeringIncrementMin;
	float	steeringIncrementMax;
	float	steeringClamp;
	float	wheelRadius;
	float	wheelWidth;
	float	wheelFriction;//BT_LARGE_FLOAT;
	float	suspensionStiffness;
	float	suspensionDamping;
	float	suspensionCompression;
	float	rollInfluence;//1.0f;

	CarPos carPos;
	btScalar carPosRot[16];
	
	std::vector<Chunk*> effects;

	btScalar suspensionRestLength;

	void CreateVehicle(Car *car);
	void CreateVehicle();
	btRigidBody*	localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape);


//FOR DEBUG ONLY. DO NOT CHANGE!
public:

	float		m_cameraHeight;

	float	m_minCameraDistance;
	float	m_maxCameraDistance;

	virtual void clientMoveAndDisplay();

	virtual void	clientResetScene();

	virtual void displayCallback();
	
	///a very basic camera following the vehicle
	virtual void updateCamera();

	virtual void specialKeyboard(int key, int x, int y);

	virtual void specialKeyboardUp(int key, int x, int y);

	void renderme();

	static DemoApplication* Create()
	{
		VehiclePhysic* demo = new VehiclePhysic();
		demo->myinit();
		demo->initPhysics();
		return demo;
	}

};


#endif