#pragma once

#include "../World/sceneComponent.h"

class World;

struct LightParameters
{
	glm::vec3 ambiant = glm::vec3(0.4f, 0.4f, 0.4f);
	glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
};

class Light : public SceneComponent
{
public:

	Light(World* newWorld) : SceneComponent(newWorld) {}
	LightParameters lightParams;

};