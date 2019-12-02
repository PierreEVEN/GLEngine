#ifndef PRIMITIVECOMPONENT_H
#define PRIMITIVECOMPONENT_H

#include "../World/sceneComponent.h"
#include <vector>

class btRigidBody;
struct btDefaultMotionState;


class PrimitiveComponent : public SceneComponent
{
	btDefaultMotionState* primitiveMotionState;
	std::vector<btRigidBody*> physicBodies;

public:

	PrimitiveComponent(World* inWorld);



};
#endif