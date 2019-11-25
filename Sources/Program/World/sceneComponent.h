#ifndef SCENECOMPONENT_H
#define SCENECOMPONENT_H

#include <glm/gtx/matrix_decompose.hpp>

class World;

class SceneComponent
{
public:

	SceneComponent(World* inWorld);
	~SceneComponent();

protected:

	World* OwningWorld;

	glm::vec3 location;
	glm::vec3 forwardVector;
	float angle;
	glm::vec3 scale3D;
	
public:

	virtual void MarkRenderStateDirty() {};

	virtual void Update(double DeltaTime) {	}

	virtual void SetLocation(glm::vec3 newLocation);
	virtual void SetForwardVector(glm::vec3 newForwardVector);
	virtual void SetAngle(float newAngle);
	virtual void SetScale3D(glm::vec3 newScale3D);

	glm::vec3 GetLocation() const;
	glm::vec3 GetForwardVector() const;
	float GetAngle() const;
	glm::vec3 GetScale3D() const;

	World* GetWorld();
};
#endif