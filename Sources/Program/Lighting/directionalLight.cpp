#include "directionalLight.h"

DirectionalLight::DirectionalLight(World* ParentWorld)
	: Light(ParentWorld)
{
	direction = glm::vec3(0.f, -1.f, 0.f);
}

