#ifndef VEHICLEPHYSIC_H
#define VEHICLEPHYSIC_H

class btVehicleTuning;
struct btVehicleRaycaster;
class btCollisionShape;

#include "BulletDynamics/Vehicle/btRaycastVehicle.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletCollision/CollisionShapes/btUniformScalingShape.h"
#include "BulletDynamics/ConstraintSolver/btConstraintSolver.h"
#include "LinearMath/btQuickprof.h"
#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btSerializer.h"

#include <glm/glm.hpp>

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


class VehiclePhysic
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
	virtual ~VehiclePhysic();
	void InitPhysics();
	void KeyDown(int key);
	void KeyUp(int key);
	void MoveVehicle();
	void ResetVehicle();
	glm::mat4 GetVehicleMatrix();
	btDynamicsWorld* GetDynamicsWorld();

private:
	///this is the most important class
	//TODO: Maybe move this to a physics class
	btDynamicsWorld*	m_dynamicsWorld;
	btScalar			m_defaultContactProcessingThreshold;

	float	gEngineForce = 0.f;
	float	gBreakingForce = 0.f;

	float	maxEngineForce = 1400.f;//this should be engine/velocity dependent
	float	maxBreakingForce = 200.f;

	float	gVehicleSteering = 0.f;
	float	steeringIncrement = 0.08f;
	float	steeringClamp = 0.4f;
	float	wheelRadius = 0.5f;
	float	wheelWidth = 0.2f;
	float	wheelFriction = 1000;//BT_LARGE_FLOAT;
	float	suspensionStiffness = 20.f;
	float	suspensionDamping = 2.3f;
	float	suspensionCompression = 4.4f;
	float	rollInfluence = 0.1f;//1.0f;

	CarPos carPos;
	btScalar carPosRot[16];


	btScalar suspensionRestLength;

	void CreateVehicle();
	btRigidBody*	localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape);
};


#endif