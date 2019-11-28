#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "Light.h"

class World;

class SpotLight : public Light
{

public:

	SpotLight(World* ParentWorld);

	glm::vec3 direction;

	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};
#endif