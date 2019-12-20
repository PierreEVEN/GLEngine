#ifndef SCENECOMPONENT_H
#define SCENECOMPONENT_H

#include <glm/gtx/matrix_decompose.hpp>
#include "../MathLibrary/vector3.h"

class World;

class SceneComponent
{
public:

	SceneComponent(World* inWorld);
	~SceneComponent();

protected:

	World* OwningWorld;

	SVector3 location;
	SRotator rotation;
	glm::vec3 forwardVector;
	float angle;
	SVector3 scale3D;
	
public:

	virtual void MarkRenderStateDirty() {};

	virtual void SetLocation(SVector3 newLocation);
	virtual void SetRotation(SRotator newRotation);
	virtual void SetForwardVector(glm::vec3 newForwardVector);
	virtual void SetAngle(float newAngle);
	virtual void SetScale3D(SVector3 newScale3D);

	virtual SVector3 GetLocation() const;
	virtual SRotator GetRotation() const;
	virtual glm::vec3 GetForwardVector() const;
	virtual float GetAngle() const;
	virtual SVector3 GetScale3D() const;

	World* GetWorld();
};
#endif