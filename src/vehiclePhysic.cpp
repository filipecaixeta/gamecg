#include <iostream>
#include <stdio.h>
#include <cmath>

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

// By default, Bullet Vehicle uses Y as up axis.

//either use heightfield or triangle mesh
#define  USE_TRIMESH_GROUND 1	
//#define  LOAD_FROM_FILE
//

int rightIndex = 0;
int upIndex = 1;
int forwardIndex = 2;
btVector3 wheelDirectionCS0(0,-1,0);
btVector3 wheelAxleCS(-1,0,0);

#include "vehiclePhysic.h"

const int maxProxies = 32766;
const int maxOverlap = 65535;

#define CUBE_HALF_EXTENT 1

VehiclePhysic::VehiclePhysic() : 
m_carChassis(0),
m_vertices(0),
m_dynamicsWorld(0),
m_defaultContactProcessingThreshold(BT_LARGE_FLOAT),
suspensionRestLength(0.6)
{
	m_vehicle = 0;
	m_wheelShape = 0;
	InitPhysics();
	for (int i = 0; i < 16; ++i)
	{
		carPosRot[i] = 0.0;
		if(i % 5 == 0)
			carPosRot[i] = 1.0;
	}
}

VehiclePhysic::~VehiclePhysic()
{
	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	for (int i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}

	delete m_indexVertexArrays;
	delete m_vertices;

	//delete dynamics world
	delete m_dynamicsWorld;

	delete m_vehicleRayCaster;

	delete m_vehicle;

	delete m_wheelShape;

	//delete solver
	delete m_constraintSolver;

	//delete broadphase
	delete m_overlappingPairCache;

	//delete dispatcher
	delete m_dispatcher;

	delete m_collisionConfiguration;
}

void VehiclePhysic::InitPhysics()
{
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	m_overlappingPairCache = new btAxisSweep3(worldMin,worldMax);
	m_constraintSolver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_constraintSolver,m_collisionConfiguration);

	btCollisionShape* groundShape = new btBoxShape(btVector3(400,3,400));
	m_collisionShapes.push_back(groundShape);

	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(0,-3.f,0));

#ifdef USE_TRIMESH_GROUND
	const float TRIANGLE_SIZE=20.f;

	//create a triangle mesh ground
	int vertStride = sizeof(btVector3);
	int indexStride = 3 * sizeof(int);

	const int NUM_VERTS_X = 20;
	const int NUM_VERTS_Y = 20;
	const int totalVerts = NUM_VERTS_X*NUM_VERTS_Y;
	const int totalTriangles = 2 * (NUM_VERTS_X - 1) * (NUM_VERTS_Y - 1);

	m_vertices = new btVector3[totalVerts];
	int* gIndices = new int[totalTriangles * 3];

	for(int i = 0; i < NUM_VERTS_X; i++)
	{
		for (int j = 0; j < NUM_VERTS_Y; j++)
		{
			float wl = 0.2f;
			float height = 0.0f;

			m_vertices[i + j * NUM_VERTS_X].setValue(
				(i - NUM_VERTS_X * 0.5f) * TRIANGLE_SIZE,
				height,
				(j - NUM_VERTS_Y * 0.5f) * TRIANGLE_SIZE
				);
		}
	}

	int index = 0;
	for(int i = 0; i < NUM_VERTS_X - 1; i++)
	{
		for (int j = 0; j < NUM_VERTS_Y - 1; j++)
		{
			gIndices[index++] = j * NUM_VERTS_X + i;
			gIndices[index++] = j * NUM_VERTS_X + i + 1;
			gIndices[index++] = (j + 1) * NUM_VERTS_X + i + 1;
			gIndices[index++] = j * NUM_VERTS_X + i;
			gIndices[index++] = (j + 1) * NUM_VERTS_X + i + 1;
			gIndices[index++] = (j + 1) * NUM_VERTS_X + i;
		}
	}

	m_indexVertexArrays = new btTriangleIndexVertexArray(totalTriangles,
		gIndices,
		indexStride,
		totalVerts,(btScalar*) &m_vertices[0].x(),vertStride);

	bool useQuantizedAabbCompression = true;
	groundShape = new btBvhTriangleMeshShape(m_indexVertexArrays,useQuantizedAabbCompression);
	
	tr.setOrigin(btVector3(0,-4.5f,0));

	m_collisionShapes.push_back(groundShape);

#else	//height field terrain shape
	int width=128;
	int length=128;


#ifdef LOAD_FROM_FILE
	unsigned char* heightfieldData = new unsigned char[width*length];
	{
		for (int i=0;i<width*length;i++)
		{
			heightfieldData[i]=0;
		}
	}

	char*	filename="heightfield128x128.cpp";
	FILE* heightfieldFile = fopen(filename,"r");
	if (!heightfieldFile)
	{
		filename="../heightfield/heightfield128x128.raw";
		heightfieldFile = fopen(filename,"r");
	}
	if (heightfieldFile)
	{
		int numBytes =fread(heightfieldData,1,width*length,heightfieldFile);
		//btAssert(numBytes);
		if (!numBytes)
		{
			printf("couldn't read heightfield at %s\n",filename);
		}
		fclose (heightfieldFile);
	}
#else
	char* heightfieldData = MyHeightfield;
#endif


	//btScalar maxHeight = 20000.f;//exposes a bug
	btScalar maxHeight = 100;
	
	bool useFloatDatam=false;
	bool flipQuadEdges=false;

	btHeightfieldTerrainShape* heightFieldShape = new btHeightfieldTerrainShape(width,length,heightfieldData,maxHeight,upIndex,useFloatDatam,flipQuadEdges);;
	btVector3 mmin,mmax;
	heightFieldShape->getAabb(btTransform::getIdentity(),mmin,mmax);

	groundShape = heightFieldShape;
	
	heightFieldShape->setUseDiamondSubdivision(true);

	btVector3 localScaling(100,1,100);
	localScaling[upIndex]=1.f;
	groundShape->setLocalScaling(localScaling);

	//tr.setOrigin(btVector3(0,9940,0));
	tr.setOrigin(btVector3(0,49.4,0));

	m_collisionShapes.push_back(groundShape);

#endif	// heightfield

	//create ground object
	localCreateRigidBody(0, tr, groundShape);
	tr.setOrigin(btVector3(0,0,0));

	btCollisionShape* chassisShape = new btBoxShape(btVector3(1.f,0.5f,2.f));
	m_collisionShapes.push_back(chassisShape);

	btCompoundShape* compound = new btCompoundShape();
	m_collisionShapes.push_back(compound);
	btTransform localTrans;
	localTrans.setIdentity();
	//localTrans effectively shifts the center of mass with respect to the chassis
	localTrans.setOrigin(btVector3(0,1,0));

	compound->addChildShape(localTrans,chassisShape);

	tr.setOrigin(btVector3(0,0.f,0));

	m_carChassis = localCreateRigidBody(800,tr,compound);//chassisShape);
	//m_carChassis->setDamping(0.2,0.2);
	
	m_wheelShape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));

	//m_collisionShapes.push_back(m_wheelShape);

	ResetVehicle();

	CreateVehicle();
}

void VehiclePhysic::CreateVehicle()
{
	m_vehicleRayCaster = new btDefaultVehicleRaycaster(m_dynamicsWorld);
	m_vehicle = new btRaycastVehicle(m_tuning,m_carChassis,m_vehicleRayCaster);
	
	///never deactivate the vehicle
	m_carChassis->setActivationState(DISABLE_DEACTIVATION);

	m_dynamicsWorld->addVehicle(m_vehicle);

	float connectionHeight = 1.2f;

	bool isFrontWheel=true;

	//choose coordinate system
	m_vehicle->setCoordinateSystem(rightIndex,upIndex,forwardIndex);

	// add wheels
    // front left
    btVector3 connectionPointCS0(CUBE_HALF_EXTENT-(0.3*wheelWidth), connectionHeight, 2*CUBE_HALF_EXTENT-wheelRadius);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
    // front right
    connectionPointCS0 = btVector3(-CUBE_HALF_EXTENT+(0.3*wheelWidth), connectionHeight, 2*CUBE_HALF_EXTENT-wheelRadius);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
    isFrontWheel = false;
    // rear right
    connectionPointCS0 = btVector3(-CUBE_HALF_EXTENT+(0.3*wheelWidth), connectionHeight, -2*CUBE_HALF_EXTENT+wheelRadius);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
    // rear left
    connectionPointCS0 = btVector3(CUBE_HALF_EXTENT-(0.3*wheelWidth), connectionHeight, -2*CUBE_HALF_EXTENT+wheelRadius);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	
	for (int i = 0; i < m_vehicle->getNumWheels(); i++)
    {
        btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
        wheel.m_suspensionStiffness = suspensionStiffness;
        wheel.m_wheelsDampingRelaxation = suspensionDamping;
        wheel.m_wheelsDampingCompression = suspensionCompression;
        wheel.m_frictionSlip = wheelFriction;
        wheel.m_rollInfluence = rollInfluence;
    }

}

void VehiclePhysic::ResetVehicle()
{
	gVehicleSteering = 0.f;
	m_carChassis->setCenterOfMassTransform(btTransform::getIdentity());
	m_carChassis->setLinearVelocity(btVector3(0,0,0));
	m_carChassis->setAngularVelocity(btVector3(0,0,0));
	m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_carChassis->getBroadphaseHandle(),GetDynamicsWorld()->getDispatcher());
	if (m_vehicle)
	{
		m_vehicle->resetSuspension();
		for (int i=0;i<m_vehicle->getNumWheels();i++)
		{
			//synchronize the wheels with the (interpolated) chassis worldtransform
			m_vehicle->updateWheelTransform(i,true);
		}
	}

}

void VehiclePhysic::MoveVehicle()
{
	{
		int wheelIndex = 2; // rear right
		m_vehicle->applyEngineForce(gEngineForce,wheelIndex);
		m_vehicle->setBrake(gBreakingForce,wheelIndex);
		wheelIndex = 3;	//rear left
		m_vehicle->applyEngineForce(gEngineForce,wheelIndex);
		m_vehicle->setBrake(gBreakingForce,wheelIndex);


		wheelIndex = 0;	//front left
		m_vehicle->setSteeringValue(gVehicleSteering,wheelIndex);
		wheelIndex = 1;	//front right
		m_vehicle->setSteeringValue(gVehicleSteering,wheelIndex);
	}

	float dt = 0.0166666f;	//60FPS

	if (m_dynamicsWorld)
	{
		int numSimSteps = m_dynamicsWorld->stepSimulation(1.0f / 60.0f, 10, 1.0f / 60.0f);

//#define VERBOSE_FEEDBACK
#ifdef VERBOSE_FEEDBACK
		if (!numSimSteps)
			printf("Interpolated transforms\n");
		else
		{
			if (numSimSteps > maxSimSubSteps)
			{
				//detect dropping frames
				printf("Dropped (%i) simulation steps out of %i\n",numSimSteps - maxSimSubSteps,numSimSteps);
			} else
			{
				printf("Simulated (%i) steps\n",numSimSteps);
			}
		}
#endif //VERBOSE_FEEDBACK
	}	

	btScalar m[16];
	btTransform transChasis;
	m_carChassis->getMotionState()->getWorldTransform(transChasis);//this gives the right position.  :D 
	transChasis.getOpenGLMatrix(m);

	std::cout << "x: " << transChasis.getOrigin().getX() << " y: " << transChasis.getOrigin().getY() << " z: " << transChasis.getOrigin().getZ() << std::endl;
	std::cout << "eng: " << gEngineForce << " brake: " << gBreakingForce << std::endl;


	carPos.chassisPos = glm::vec3(
		transChasis.getOrigin().getX(),
		transChasis.getOrigin().getY(),
		transChasis.getOrigin().getZ()
		);

	float AngleX, AngleY, AngleZ;

	for(int i = 0; i < 16; i++)
	{
		carPosRot[i] = m[i];
	}
	AngleX = atan2f(m[1], m[5]);  // 
	AngleY = atan2f(m[8], m[10]);  // Y-axis!!!
	AngleZ = asinf(m[9]);

	std::cout << "Rx: " << AngleX << " Ry: " << AngleY << " Rz: " << AngleZ << std::endl;

	carPos.chassisRot = glm::vec3(AngleX, AngleY, AngleZ);

	btTransform transWheels;
	for (int i = 0; i < m_vehicle->getNumWheels(); i++)
	{
		//synchronize the wheels with the (interpolated) chassis worldtransform
		m_vehicle->updateWheelTransform(i,true);

		m_vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(m);

		/*carPos.wheels[i].pos = glm::vec3(
			transWheels.getOrigin().getX(),
			transWheels.getOrigin().getY(),
			transWheels.getOrigin().getZ()
			);*/


		AngleX = atan2f(m[1], m[5]);  // 
		AngleY = atan2f(m[8], m[10]);  // Y-axis!!!
		AngleZ = asinf(m[9]);

		std::cout << "Wx: " << AngleX << " Wy: " << AngleY << " Wz: " << AngleZ << std::endl;
		carPos.wheels[i].rot = glm::vec3(AngleX, AngleY, AngleZ);
	}
}

void VehiclePhysic::KeyDown(int key)
{
	if(key == KEY_LEFT) 
	{
		gVehicleSteering += steeringIncrement;
		if (gVehicleSteering > steeringClamp)
				gVehicleSteering = steeringClamp;
	}
    if(key == KEY_RIGHT) 
	{
		gVehicleSteering -= steeringIncrement;
		if (gVehicleSteering < -steeringClamp)
				gVehicleSteering = -steeringClamp;
	}
    if(key == KEY_FORWARD)
	{
		gEngineForce = maxEngineForce;
		gBreakingForce = 0.f;
	}
	if(key == KEY_BACK)
	{			
		gEngineForce = -maxEngineForce;
		gBreakingForce = 0.f;
	}
	if(key == KEY_SPACE)
	{
		gBreakingForce = maxBreakingForce;
		gEngineForce = 0.0;
	}
}

void VehiclePhysic::KeyUp(int key)
{
	if(key == KEY_LEFT || key == KEY_RIGHT) 
	{
		gVehicleSteering = 0.0;
	}
    if(key == KEY_FORWARD || key == KEY_BACK)
	{
		gEngineForce = 0.0;
	}
	if(key == KEY_SPACE)
	{
		gBreakingForce = 0.0;
	}
}

btRigidBody* VehiclePhysic::localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia(mass,localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
	btRigidBody* body = new btRigidBody(mass,0,shape,localInertia);
	body->setWorldTransform(startTransform);
#endif//

	m_dynamicsWorld->addRigidBody(body);

	return body;
}

glm::mat4 VehiclePhysic::GetVehicleMatrix()
{
	glm::mat4 ret(
		(float)carPosRot[0],
		(float)carPosRot[1],
		(float)carPosRot[2],
		(float)carPosRot[3],
		(float)carPosRot[4],
		(float)carPosRot[5],
		(float)carPosRot[6],
		(float)carPosRot[7],
		(float)carPosRot[8],
		(float)carPosRot[9],
		(float)carPosRot[10],
		(float)carPosRot[11],
		(float)carPosRot[12],
		(float)carPosRot[13],
		(float)carPosRot[14],
		(float)carPosRot[15]
		);
	return ret;
}

btDynamicsWorld* VehiclePhysic::GetDynamicsWorld()
{
	return m_dynamicsWorld;
}