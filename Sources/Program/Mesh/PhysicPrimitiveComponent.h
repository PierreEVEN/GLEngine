#pragma once

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
};