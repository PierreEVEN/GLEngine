#include "sceneComponent.h"
#include "world.h"

SceneComponent::SceneComponent(World* inWorld)
{
	location = glm::vec3(0);
	forwardVector = glm::vec3(1.f, 0.f, 0.f);
	angle = 0.f;
	scale3D = glm::vec3(1);
	OwningWorld = inWorld;
	OwningWorld->AddPrimitive(this);
}

SceneComponent::~SceneComponent() { }

void SceneComponent::SetLocation(glm::vec3 newLocation) { location = newLocation; }
void SceneComponent::SetForwardVector(glm::vec3 newForwardVector) { forwardVector = newForwardVector; }
void SceneComponent::SetAngle(float newAngle) { angle = newAngle; }
void SceneComponent::SetScale3D(glm::vec3 newScale3D) { scale3D = newScale3D; }

glm::vec3 SceneComponent::GetLocation() const { return location; }
glm::vec3 SceneComponent::GetForwardVector() const { return forwardVector; }
float SceneComponent::GetAngle() const { return angle; }
glm::vec3 SceneComponent::GetScale3D() const { return scale3D; }

World* SceneComponent::GetWorld() { return OwningWorld; }
