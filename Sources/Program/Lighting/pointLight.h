#include "Light.h"


class World;


class PointLight : public Light
{

private:


public:

	PointLight(World* newWorld);

	virtual void MarkRenderStateDirty() override;

	float constant;
	float linear;
	float quadratic;


};