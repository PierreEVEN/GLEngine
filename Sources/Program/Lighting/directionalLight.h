#pragma once

#include "Light.h"

class World;

struct DirectionalLightParameters
{
	glm::vec4 direction = glm::vec4(0.f, 0.f, -1.f, 0.f);
};

class DirectionalLight : public Light
{

public:

	DirectionalLight(Scene* drawScene);
	DirectionalLightParameters directionalLightParams;
};