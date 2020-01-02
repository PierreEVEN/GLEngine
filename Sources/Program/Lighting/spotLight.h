#pragma once

#include "Light.h"

class World;

struct SpotLightParameters
{
	glm::vec4 direction = glm::vec4(0.f, -1.f, 0.f, 0.f);
	glm::vec4 lightPosition = glm::vec4(0, 0, 0, 0);
	float constant = 1.f;
	float linear = 0.09f;
	float quadratic = 0.032f;
	float cutOff = glm::cos(glm::radians(12.5f));
	float outerCutOff = glm::cos(glm::radians(15.f));
};

class SpotLight : public Light
{
public:
	SpotLight(Scene* drawScene);
	SpotLightParameters spotLightParams;
};