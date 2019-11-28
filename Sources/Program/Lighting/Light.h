#ifndef LIGHT_H
#define LIGHT_H

#include "../World/sceneComponent.h"


class World;

class Light : public SceneComponent
{
public:

	Light(World* newWorld);

	glm::vec3 ambiant;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 lightColor;

};
#endif