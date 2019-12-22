#pragma once

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