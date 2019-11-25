#include "pointLight.h"
#include "../World/world.h"

PointLight::PointLight(World* newWorld)
	: Light(newWorld) {}

void PointLight::MarkRenderStateDirty()
{

}