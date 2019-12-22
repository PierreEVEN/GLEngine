#include "spotLight.h"
#include "../World/world.h"

SpotLight::SpotLight(World* ParentWorld)
	: Light(ParentWorld)
{
	ParentWorld->RegisterSpotLight(this);
}