#pragma once

#include "Light.h"

class World;

struct SpotLightParameters
{
	float constant = 1.f;
	float linear = 0.09f;
	float quadratic = 0.032f;
	float cutOff = glm::cos(glm::radians(12.5f));
	float outerCutOff = glm::cos(glm::radians(15.f));
	glm::vec3 direction = glm::vec3(0.f, -1.f, 0.f);
};

class SpotLight : public Light
{
public:
	SpotLight(World* ParentWorld);
	SpotLightParameters spotLightParams;
};