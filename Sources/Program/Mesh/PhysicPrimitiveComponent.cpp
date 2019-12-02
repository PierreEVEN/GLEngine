#include "PhysicPrimitiveComponent.h"
#include "../World/world.h"
#include <bullet3D/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <iostream>
#include <glfw3/glfw3.h>


void PhysicPrimitiveComponent::Init(World* inWorld)
{


	btCollisionShape* shape = new btBoxShape(btVector3(1, 1, 1));

	btTransform myTransform;
	myTransform.setIdentity();
	myTransform.setOrigin(btVector3(0, 0, 10));

	btVector3 localInertia(0, 0, 0);
	btScalar mass = 0.5f;
	shape->calculateLocalInertia(mass, localInertia);



	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	myMotionState = new btDefaultMotionState(myTransform);
	btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(mass, myMotionState, shape, localInertia);
	body = new btRigidBody(myBoxRigidBodyConstructionInfo);

	inWorld->GetPhysicWorld()->addRigidBody(body);

}

PhysicPrimitiveComponent::PhysicPrimitiveComponent(World* inWorld)
	: MeshSectionComponent(inWorld)
{
	Init(inWorld);
}

PhysicPrimitiveComponent::PhysicPrimitiveComponent(World* inWorld, StaticMeshSection* inStaticMeshSection)
	: MeshSectionComponent(inWorld, inStaticMeshSection)
{
	Init(inWorld);
}

void PhysicPrimitiveComponent::SetLocation(SVector3 newLocation)
{
	MeshSectionComponent::SetLocation(newLocation);
	btTransform newTransform;
	newTransform.setIdentity();
	newTransform.setOrigin(newLocation.ToBulletVector());
	body->setWorldTransform(newTransform);
}

SVector3 PhysicPrimitiveComponent::GetLocation() const
{
	btTransform objectTransform = body->getWorldTransform();	
	return glm::vec3((float)objectTransform.getOrigin().getX(), (float)objectTransform.getOrigin().getY(), (float)objectTransform.getOrigin().getZ());
	return MeshSectionComponent::GetLocation();
}

float PhysicPrimitiveComponent::GetAngle() const
{
	btQuaternion objectRotation = body->getWorldTransform().getRotation();
	return (objectRotation.getAngle() * 180.f) / glm::pi<float>();
}

glm::vec3 PhysicPrimitiveComponent::GetForwardVector() const
{
	btQuaternion objectRotation = body->getWorldTransform().getRotation();
	return glm::vec3((float)objectRotation.getAxis().getX(), (float)objectRotation.getAxis().getY(), (float)objectRotation.getAxis().getZ());
}
