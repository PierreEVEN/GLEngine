#include "pointLight.h"
#include "../World/world.h"

PointLight::PointLight(World* newWorld)
	: Light(newWorld) {

	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;

	newWorld->RegisterPointLight(this);
}

void PointLight::MarkRenderStateDirty()
{

}