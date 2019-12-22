#pragma once

#include "Light.h"

class World;

struct DirectionalLightParameters
{
	glm::vec3 direction = glm::vec3(0.f, -1.f, 0.f);
};

class DirectionalLight : public Light
{

public:

	DirectionalLight(World* ParentWorld);
	DirectionalLightParameters directionalLightParams;
};