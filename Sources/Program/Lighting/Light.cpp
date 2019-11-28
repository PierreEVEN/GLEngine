#include "Light.h"
#include "../World/world.h"

Light::Light(World* newWorld) 
	: SceneComponent(newWorld) 
{

	ambiant = glm::vec3(0.4f, 0.4f, 0.4f);
	diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	specular = glm::vec3(1.0f, 1.0f, 1.0f);
}

