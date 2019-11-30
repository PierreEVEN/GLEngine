#include "spotLight.h"
#include "../World/world.h"

SpotLight::SpotLight(World* ParentWorld)
	: Light(ParentWorld)
{
	constant = 1.f;
	linear = 0.09f;
	quadratic = 0.032f;
	cutOff = glm::cos(glm::radians(12.5f));
	outerCutOff = glm::cos(glm::radians(15.f));
	ParentWorld->RegisterSpotLight(this);
}