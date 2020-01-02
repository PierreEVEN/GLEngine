#pragma once

#include "Light.h"

class World;

struct PointLightParameters
{
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
	int offset;
	glm::vec4 lightPosition = glm::vec4(0);
};

class PointLight : public Light
{
public:
	PointLight(Scene* drawScene);
	PointLightParameters pointLightParams;
};