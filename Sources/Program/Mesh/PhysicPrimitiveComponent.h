#ifndef PHYSICPRIMITIVECOMPONENT_H
#define PHYSICPRIMITIVECOMPONENT_H

#include "meshSectionComponent.h"

#include <bullet3D/btBulletDynamicsCommon.h>


class PhysicPrimitiveComponent : public MeshSectionComponent
{


	// btDefaultMotionState fournit une implémentation pour synchroniser les transformations.
	btDefaultMotionState *myMotionState;
	btRigidBody *body;


	void Init(World* inWorld);

	
public:
	PhysicPrimitiveComponent(World* inWorld);
	PhysicPrimitiveComponent(World* inWorld, StaticMeshSection* inStaticMeshSection);


	virtual void SetLocation(SVector3 newLocation) override;
	virtual SVector3 GetLocation() const override;
	virtual float GetAngle() const override;
	virtual glm::vec3 GetForwardVector() const override;

protected:

	void BounceCoordinate(double Coord, double& speed, float& RotSpeed, float posMax, float posMin, float restitution = 1.f)
	{
		RotSpeed = Coord < posMin ? RotSpeed * restitution : Coord > posMax ? RotSpeed * restitution : RotSpeed;;
		speed = Coord < posMin ? -speed * restitution : Coord > posMax ? -speed * restitution : speed;
	}

	void DragVelocity(double& Velocity, float Drag, double DeltaTime)
	{
		Velocity -= Velocity * DeltaTime * Drag;
	}

	void SwapCoordinate(double& Coord, float posMax, float posMin)
	{
		Coord = Coord < posMin ? posMax : Coord > posMax ? posMin : Coord;
	}



};
#endif