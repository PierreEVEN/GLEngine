#pragma once

#include "../World/sceneComponent.h"
#include <vector>

class btRigidBody;
struct btDefaultMotionState;
struct SVector3;

struct BoxSphereBound
{
public:
	BoxSphereBound() {
		BoxSphereBound(SVector3(0), 0);
	}
	BoxSphereBound(SVector3 objectPos) : boxMin(objectPos), boxMax(objectPos)
	{
		boundRadius = 0;
	}
	BoxSphereBound(SVector3 objectPos, float radius)
		: BoxSphereBound(objectPos)
	{
		boundRadius = radius;
	}
	BoxSphereBound(SVector3 min, SVector3 max);
private:
	float boundRadius;
	SVector3 boxMin;
	SVector3 boxMax;
public:
	float GetBoundRadius() const { return boundRadius; }
	SVector3 GetOrigin() const;
	SVector3 GetBoxMin() const { return boxMin; }
	SVector3 GetBoxMax() const { return boxMax; }
};

class PrimitiveComponent : public SceneComponent
{
	btDefaultMotionState* primitiveMotionState;
	std::vector<btRigidBody*> physicBodies;

protected:

	BoxSphereBound localBounds;
	BoxSphereBound worldBounds;

public:

	PrimitiveComponent(Scene* inScene);
	~PrimitiveComponent();

	virtual void Tick() override;
	virtual void RebuildTransformData()override;
	virtual void RebuildLocalBounds() { RebuildWorldBounds(); }
	virtual void RebuildWorldBounds();
	BoxSphereBound GetLocalBounds() const { return localBounds; }
	BoxSphereBound GetWorldBounds() const { return worldBounds; }
	virtual void DrawEditorWindow() override;
};