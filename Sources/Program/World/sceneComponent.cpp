#include "sceneComponent.h"
#include "world.h"

SceneComponent::SceneComponent(World* inWorld)
{
	location = SVector3(0);
	rotation = SRotator(0);
	forwardVector = glm::vec3(1.f, 0.f, 0.f);
	angle = 0.f;
	scale3D = SVector3(1);
	OwningWorld = inWorld;
	OwningWorld->RegisterPrimitive(this);
}

SceneComponent::~SceneComponent() { }

void SceneComponent::SetLocation(SVector3 newLocation) { location = newLocation; }
void SceneComponent::SetRotation(SRotator newRotation) { rotation = newRotation; }
void SceneComponent::SetForwardVector(glm::vec3 newForwardVector) { forwardVector = newForwardVector; }
void SceneComponent::SetAngle(float newAngle) { angle = newAngle; }
void SceneComponent::SetScale3D(SVector3 newScale3D) { scale3D = newScale3D; }

SVector3 SceneComponent::GetLocation() const { return location; }
SRotator SceneComponent::GetRotation() const { return rotation; }
glm::vec3 SceneComponent::GetForwardVector() const { return forwardVector; }
float SceneComponent::GetAngle() const { return angle; }
SVector3 SceneComponent::GetScale3D() const { return scale3D; }

World* SceneComponent::GetWorld() { return OwningWorld; }
