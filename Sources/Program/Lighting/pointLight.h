#pragma once

#include "Light.h"

class World;

struct PointLightParameters
{
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
};

class PointLight : public Light
{
public:
	PointLight(World* newWorld);
	PointLightParameters pointLightParams;
};