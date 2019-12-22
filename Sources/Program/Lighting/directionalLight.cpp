#include "directionalLight.h"
#include "../World/world.h"

DirectionalLight::DirectionalLight(World* ParentWorld)
	: Light(ParentWorld)
{
	ParentWorld->RegisterDirectionalLight(this);
}

