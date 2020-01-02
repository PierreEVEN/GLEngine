#pragma once

#include "../World/sceneComponent.h"

class World;

struct LightParameters
{
	glm::vec4 ambiant = glm::vec4(0.1f, 0.1f, 0.1f, 0.f);
	glm::vec4 diffuse = glm::vec4(1.f, 1.f, 1.f, 0.f);
	glm::vec4 specular = glm::vec4(1.0f, 1.0f, 1.0f, 0.f);
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.f);
};

class Light : public SceneComponent
{
public:
	int randIndex;

	Light(Scene* drawScene);
	LightParameters lightParams;
	virtual void Tick() override;
};