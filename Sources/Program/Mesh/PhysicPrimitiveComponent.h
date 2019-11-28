#ifndef PHYSICPRIMITIVECOMPONENT_H
#define PHYSICPRIMITIVECOMPONENT_H

#include "meshSectionComponent.h"

class PhysicPrimitiveComponent : public MeshSectionComponent
{

	glm::vec3 LinearVelocity;
	glm::vec3 AngularVelocity;
	
public:
	PhysicPrimitiveComponent(World* inWorld);
	PhysicPrimitiveComponent(World* inWorld, StaticMeshSection* inStaticMeshSection);



	void Init()
	{
		LinearVelocity = glm::vec3(0.f);
		AngularVelocity = glm::vec3(0.f);
	}

	virtual void Update(double DeltaTime) override
	{
		LinearVelocity += glm::vec3(0.f, -(float)DeltaTime * 5.f, 0.f);
		SetLocation(location + LinearVelocity * glm::vec3((float)DeltaTime));
		if (location[1] < 1.f)
		{
			location[1] = 1.01f;
			LinearVelocity *= glm::vec3(1, -.9, 1);
		}
	}

	void SetLinearVelocity(glm::vec3 newVelocity)
	{
		LinearVelocity = newVelocity;
	}
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