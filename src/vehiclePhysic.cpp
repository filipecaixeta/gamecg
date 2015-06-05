#include <iostream>
#include <stdio.h>
#include <cmath>

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"


// By default, Bullet Vehicle uses Y as up axis.

int rightIndex = 0;
int upIndex = 1;
int forwardIndex = 2;
btVector3 wheelDirectionCS0(0,-1,0);
btVector3 wheelAxleCS(-1,0,0);

#include "vehiclePhysic.h"

const int maxProxies = 32766;
const int maxOverlap = 65535;

#define CUBE_HALF_EXTENT 1

VehiclePhysic::VehiclePhysic(Scenario *scenario,Car *car): 
m_carChassis(0),
m_vertices(0),
m_cameraHeight(4.f),
m_minCameraDistance(3.f),
m_maxCameraDistance(10.f),
gEngineForce(0.f),
gBreakingForce(0.f),
maxEngineForce(10000.f),//this should be engine/velocity dependent
maxBreakingForce(200.f),
gVehicleSteering(0.f),
steeringIncrement(0.04f),
steeringIncrementMin(0.0008f),
steeringIncrementMax(0.04f),
steeringClamp(0.35f),
wheelRadius(0.5f),
wheelWidth(0.2f),
wheelFriction(1000),//BT_LARGE_
suspensionStiffness(20.f),
suspensionDamping(2.3f),
suspensionCompression(4.4f),
rollInfluence(0.1f),//1.0f;
suspensionRestLength(0.6)
{

	vec3 wMin=scenario->model.cmin;
	vec3 wMax=scenario->model.cmax;
	btVector3 worldMin(wMin.x,wMin.y,wMin.z);
	btVector3 worldMax(wMax.x,wMax.y,wMax.z);

	m_vehicle = 0;
	m_wheelShape = 0;
	m_cameraPosition = btVector3(30,30,30);

	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);


	m_overlappingPairCache = new btAxisSweep3(worldMin,worldMax);
	m_constraintSolver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_constraintSolver,m_collisionConfiguration);
	
	btTransform tr;

	for (Mesh &m: scenario->model.meshes)
	{
		if (m.meshName.find("no_colision")==0)
			continue;
		Mesh* meshP;
		meshP=&m;

		int totalVerts = meshP->vertices.size();
		int totalTriangles = meshP->indices.size()/3;

		int vertStride = sizeof(Vertex);
		int indexStride = 3*sizeof(int);

		int* gIndices = (int*)&(meshP->indices[0]);
		btScalar *verticesBeg = (btScalar*)&(meshP->vertices[0].Position.x);

		m_indexVertexArrays = new btTriangleIndexVertexArray(totalTriangles,
		gIndices,
		indexStride,
		totalVerts,
		verticesBeg,
		vertStride);

		bool useQuantizedAabbCompression = true;
		btCollisionShape* groundShape = new btBvhTriangleMeshShape(m_indexVertexArrays,useQuantizedAabbCompression);
		
		tr.setIdentity();
		tr.setOrigin(btVector3(0,0.f,0));

		m_collisionShapes.push_back(groundShape);
		
		localCreateRigidBody(0, tr, groundShape);
		tr.setOrigin(btVector3(0,0,0));
	}

	vec3 sizeCar=vec3(car->model.cmax-car->model.cmin)/2.0f;
	std::cout << sizeCar.x << " " << sizeCar.y << " " << sizeCar.z << std::endl;
	btCollisionShape* chassisShape = new btBoxShape(btVector3(sizeCar.x,sizeCar.y,sizeCar.z));
	m_collisionShapes.push_back(chassisShape);

	btCompoundShape* compound = new btCompoundShape();
	m_collisionShapes.push_back(compound);
	btTransform localTrans;
	localTrans.setIdentity();
	//localTrans effectively shifts the center of mass with respect to the chassis
	localTrans.setOrigin(btVector3(0,1,0));

	compound->addChildShape(localTrans,chassisShape);

	tr.setOrigin(btVector3(0.0f,0.0f,0.0f));

	m_carChassis = localCreateRigidBody(1000,tr,compound);//chassisShape);
	
	m_wheelShape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));

	ResetVehicle();

	CreateVehicle(car);

	for (int i = 0; i < 16; ++i)
	{
		carPosRot[i] = 0.0;
		if(i % 5 == 0)
			carPosRot[i] = 1.0;
	}

	//Start Sounds
	Chunk *tmp;
	tmp = new Chunk(22050);
	tmp->loadFile("../sound/effect/start1.wav");
	effect.push_back(tmp);
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

void VehiclePhysic::CreateVehicle(Car *car)
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
    btVector3 connectionPointCS0(car->Tire_fl_center.x,car->Tire_fl_center.y,car->Tire_fl_center.z);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
    // front right
    connectionPointCS0 = btVector3(car->Tire_fr_center.x,car->Tire_fr_center.y,car->Tire_fr_center.z);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
    isFrontWheel = false;
    // rear right
    connectionPointCS0 = btVector3(car->Tire_br_center.x,car->Tire_br_center.y,car->Tire_br_center.z);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
    // rear left
    connectionPointCS0 = btVector3(car->Tire_bl_center.x,car->Tire_bl_center.y,car->Tire_bl_center.z);
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
	btTransform cm;
	cm.setIdentity();
	// cm.setOrigin(btVector3(5.0f,00.0f,5.0f));
	m_carChassis->setCenterOfMassTransform(cm);
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

	std::cout << "KmHour: " << m_vehicle->getCurrentSpeedKmHour() << std::endl;

	float dt = 0.0166666f;	//60FPS

	if (m_dynamicsWorld)
	{
		int numSimSteps = m_dynamicsWorld->stepSimulation(dt, 2);
	}	

	btScalar m[16];
	btTransform transChasis;
	m_carChassis->getMotionState()->getWorldTransform(transChasis);//this gives the right position.  :D 
	transChasis.getOpenGLMatrix(m);

	//std::cout << "x: " << transChasis.getOrigin().getX() << " y: " << transChasis.getOrigin().getY() << " z: " << transChasis.getOrigin().getZ() << std::endl;
	//std::cout << "eng: " << gEngineForce << " brake: " << gBreakingForce << std::endl;


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

	//std::cout << "Rx: " << AngleX << " Ry: " << AngleY << " Rz: " << AngleZ << std::endl;

	carPos.chassisRot = glm::vec3(AngleX, AngleY, AngleZ);

	btTransform transWheels;
	for (int i = 0; i < m_vehicle->getNumWheels(); i++)
	{
		//synchronize the wheels with the (interpolated) chassis worldtransform
		m_vehicle->updateWheelTransform(i,true);

		m_vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(m);
		transWheels = m_vehicle->getWheelTransformWS(i);
		carPos.wheels[i].pos = glm::vec3(
			transWheels.getOrigin().getX(),
			transWheels.getOrigin().getY(),
			transWheels.getOrigin().getZ()
			);


		AngleX = atan2f(m[1], m[5]);  // 
		AngleY = atan2f(m[8], m[10]);  // Y-axis!!!
		AngleZ = asinf(m[9]);

		//std::cout << "Wx: " << AngleX << " Wy: " << AngleY << " Wz: " << AngleZ << std::endl;
		carPos.wheels[i].rot = glm::vec3(AngleX, AngleY, AngleZ);
	}
}

void VehiclePhysic::KeyDown(int key)
{
	int fat = 3;
	if(key == KEY_LEFT) 
	{
		if(m_vehicle->getCurrentSpeedKmHour() > 0)
			steeringIncrement = pow((m_vehicle->getCurrentSpeedKmHour() * (steeringIncrementMin - steeringIncrementMax) + 200.0 * steeringIncrementMax) / 200.0, fat) * steeringIncrementMax / (pow(steeringIncrementMax,fat) - pow(steeringIncrementMin,fat));
		if (steeringIncrement < steeringIncrementMin)
			steeringIncrement = steeringIncrementMin;
		gVehicleSteering += steeringIncrement;
		if (gVehicleSteering > steeringClamp)
				gVehicleSteering = steeringClamp;
	}
    if(key == KEY_RIGHT) 
	{
		if(m_vehicle->getCurrentSpeedKmHour() > 0)
			steeringIncrement = pow((m_vehicle->getCurrentSpeedKmHour() * (steeringIncrementMin - steeringIncrementMax) + 200.0 * steeringIncrementMax) / 200.0, fat) * steeringIncrementMax / (pow(steeringIncrementMax,fat) - pow(steeringIncrementMin,fat));
		if (steeringIncrement < steeringIncrementMin)
			steeringIncrement = steeringIncrementMin;
		gVehicleSteering -= steeringIncrement;
		if (gVehicleSteering < -steeringClamp)
				gVehicleSteering = -steeringClamp;
	}
    if(key == KEY_FORWARD)
	{
		gEngineForce = maxEngineForce - m_vehicle->getCurrentSpeedKmHour() * 50.0;
		if(gEngineForce < 2000.0f)
			gEngineForce = 2000.0f;
		if(m_vehicle->getCurrentSpeedKmHour() < 2.0)
		{
			effect[0]->playSound();
		}
		gBreakingForce = 0.f;
	}
	if(key == KEY_BACK)
	{			
		gEngineForce = -maxEngineForce;
		if(m_vehicle->getCurrentSpeedKmHour() > 0)
		{
			gBreakingForce = maxBreakingForce;
		}
		else
		{
			gBreakingForce = 0.0f;
		}
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
		gVehicleSteering *= 0.2;
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

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

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



//USED ONLY FOR DEBUG! DO NOT CHANGE

void VehiclePhysic::initPhysics()
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

	// Create Mesh **********************************************
	const float TRIANGLE_SIZE=20.f;

	//create a triangle mesh ground
	int vertStride = sizeof(btVector3);
	int indexStride = 3 * sizeof(int);

	const int NUM_VERTS_X = 20;
	const int NUM_VERTS_Y = 20;
	const int totalVerts = NUM_VERTS_X*NUM_VERTS_Y;
	const int totalTriangles = 2 * (NUM_VERTS_X - 1) * (NUM_VERTS_Y - 1);

	m_vertices = new btVector3[totalVerts];
	int* gIndices = new int[totalTriangles*3];

	for(int i = 0; i < NUM_VERTS_X; i++)
	{
		for (int j = 0; j < NUM_VERTS_Y; j++)
		{
			float wl = 0.2f;
			float height = 60.f*sinf(float(i)*wl)*cosf(float(j)*wl);//0.0f;

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
	
	tr.setOrigin(btVector3(0,0.f,0));

	m_collisionShapes.push_back(groundShape);

	//********************************************************************

	//CreateTerrain(tr, groundShape);
	//create ground object
	localCreateRigidBody(0, tr, groundShape);
	tr.setOrigin(btVector3(0,0,0));

	btCollisionShape* chassisShape = new btBoxShape(btVector3(1.45256f,0.92366f,3.44712f));
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
	
	m_wheelShape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));

	ResetVehicle();

	CreateVehicle();
}

VehiclePhysic::VehiclePhysic() : 
m_carChassis(0),
m_vertices(0),
m_cameraHeight(4.f),
m_minCameraDistance(3.f),
m_maxCameraDistance(10.f),
gEngineForce(0.f),
gBreakingForce(0.f),
maxEngineForce(2000.f),//this should be engine/velocity dependent
maxBreakingForce(200.f),
gVehicleSteering(0.f),
steeringIncrement(0.04f),
steeringIncrementMin(0.0005f),
steeringIncrementMax(0.04f),
steeringClamp(0.3f),
wheelRadius(0.5f),
wheelWidth(0.2f),
wheelFriction(1000),//BT_LARGE_
suspensionStiffness(20.f),
suspensionDamping(2.3f),
suspensionCompression(4.4f),
rollInfluence(0.1f),//1.0f;
suspensionRestLength(1.0)
{
	m_vehicle = 0;
	m_wheelShape = 0;
	m_cameraPosition = btVector3(30,30,30);
	initPhysics();
	for (int i = 0; i < 16; ++i)
	{
		carPosRot[i] = 0.0;
		if(i % 5 == 0)
			carPosRot[i] = 1.0;
	}
}

//to be implemented by the demo
void VehiclePhysic::renderme()
{
	
	updateCamera();

	btScalar m[16];
	int i;


	btVector3 wheelColor(1,0,0);

	btVector3	worldBoundsMin,worldBoundsMax;
	getDynamicsWorld()->getBroadphase()->getBroadphaseAabb(worldBoundsMin,worldBoundsMax);



	for (i=0;i<m_vehicle->getNumWheels();i++)
	{
		//synchronize the wheels with the (interpolated) chassis worldtransform
		m_vehicle->updateWheelTransform(i,true);
		//draw wheels (cylinders)
		m_vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(m);
		
		float AngleX, AngleY, AngleZ;
		AngleX = atan2f(m[1], m[5]);  // 
		AngleY = atan2f(m[8], m[10]);  // Y-axis!!!
		AngleZ = asinf(m[9]);
		
		std::cout << "Wx: " << AngleX << " Wy: " << AngleY << " Wz: " << AngleZ << std::endl;

		m_shapeDrawer->drawOpenGL(m,m_wheelShape,wheelColor,getDebugMode(),worldBoundsMin,worldBoundsMax);
	}


	DemoApplication::renderme();

}

void VehiclePhysic::clientMoveAndDisplay()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	
	{			
		int wheelIndex = 2;
		m_vehicle->applyEngineForce(gEngineForce,wheelIndex);
		m_vehicle->setBrake(gBreakingForce,wheelIndex);
		wheelIndex = 3;
		m_vehicle->applyEngineForce(gEngineForce,wheelIndex);
		m_vehicle->setBrake(gBreakingForce,wheelIndex);


		wheelIndex = 0;
		m_vehicle->setSteeringValue(gVehicleSteering,wheelIndex);
		wheelIndex = 1;
		m_vehicle->setSteeringValue(gVehicleSteering,wheelIndex);

	}


	float dt = getDeltaTimeMicroseconds() * 0.000001f;
	
	if (m_dynamicsWorld)
	{
		//during idle mode, just run 1 simulation step maximum
		int maxSimSubSteps = m_idle ? 1 : 2;
		if (m_idle)
			dt = 1.0/420.f;

		int numSimSteps = m_dynamicsWorld->stepSimulation(dt,maxSimSubSteps);
		
	}

	
	

	renderme(); 

	//optional but useful: debug drawing
	if (m_dynamicsWorld)
		m_dynamicsWorld->debugDrawWorld();

	glFlush();
	glutSwapBuffers();
}

void VehiclePhysic::displayCallback(void) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	renderme();

	//optional but useful: debug drawing
	if (m_dynamicsWorld)
		m_dynamicsWorld->debugDrawWorld();

	glFlush();
	glutSwapBuffers();
}

void VehiclePhysic::clientResetScene()
{
	gVehicleSteering = 0.f;
	m_carChassis->setCenterOfMassTransform(btTransform::getIdentity());
	m_carChassis->setLinearVelocity(btVector3(0,0,0));
	m_carChassis->setAngularVelocity(btVector3(0,0,0));
	m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_carChassis->getBroadphaseHandle(),getDynamicsWorld()->getDispatcher());
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

void VehiclePhysic::specialKeyboardUp(int key, int x, int y)
{
   switch (key) 
    {
    case GLUT_KEY_UP :
		{
			gEngineForce = 0.f;
		break;
		}
	case GLUT_KEY_DOWN :
		{			
			gEngineForce = 0.f; 
		break;
		}
    case GLUT_KEY_LEFT : 
		{
			gVehicleSteering = 0.f;

		break;
		}
    case GLUT_KEY_RIGHT : 
		{
			gVehicleSteering = 0.f;
		break;
		}
	default:
		DemoApplication::specialKeyboardUp(key,x,y);
        break;
    }
}

void VehiclePhysic::specialKeyboard(int key, int x, int y)
{

	//	printf("key = %i x=%i y=%i\n",key,x,y);

    switch (key) 
    {
    case GLUT_KEY_LEFT : 
		{
			gVehicleSteering += steeringIncrement;
			if (	gVehicleSteering > steeringClamp)
					gVehicleSteering = steeringClamp;

		break;
		}
    case GLUT_KEY_RIGHT : 
		{
			gVehicleSteering -= steeringIncrement;
			if (	gVehicleSteering < -steeringClamp)
					gVehicleSteering = -steeringClamp;

		break;
		}
    case GLUT_KEY_UP :
		{
			gEngineForce = maxEngineForce;
			gBreakingForce = 0.f;
		break;
		}
	case GLUT_KEY_DOWN :
		{			
			gEngineForce = -maxEngineForce;
			gBreakingForce = 10.f;
		break;
		}
	default:
		DemoApplication::specialKeyboard(key,x,y);
        break;
    }

	glutPostRedisplay();
}

void	VehiclePhysic::updateCamera()
{


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	btTransform chassisWorldTrans;

	//look at the vehicle
	m_carChassis->getMotionState()->getWorldTransform(chassisWorldTrans);
	m_cameraTargetPosition = chassisWorldTrans.getOrigin();

	m_cameraPosition[1] = (15.0*m_cameraPosition[1] + m_cameraTargetPosition[1] + m_cameraHeight)/16.0;

	btVector3 camToObject = m_cameraTargetPosition - m_cameraPosition;

	//keep distance between min and max distance
	float cameraDistance = camToObject.length();
	float correctionFactor = 0.f;
	if (cameraDistance < m_minCameraDistance)
	{
		correctionFactor = 0.15*(m_minCameraDistance-cameraDistance)/cameraDistance;
	}
	if (cameraDistance > m_maxCameraDistance)
	{
		correctionFactor = 0.15*(m_maxCameraDistance-cameraDistance)/cameraDistance;
	}
	m_cameraPosition -= correctionFactor*camToObject;
	
	  btScalar aspect = m_glutScreenWidth / (btScalar)m_glutScreenHeight;
        glFrustum (-aspect, aspect, -1.0, 1.0, 1.0, 10000.0);

         glMatrixMode(GL_MODELVIEW);
         glLoadIdentity();

    gluLookAt(m_cameraPosition[0],m_cameraPosition[1],m_cameraPosition[2],
                      m_cameraTargetPosition[0],m_cameraTargetPosition[1], m_cameraTargetPosition[2],
                          m_cameraUp.getX(),m_cameraUp.getY(),m_cameraUp.getZ());
}
