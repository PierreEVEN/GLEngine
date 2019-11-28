#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

class World;

class DirectionalLight : public Light
{

public:

	DirectionalLight(World* ParentWorld);

	glm::vec3 direction;
};
#endif