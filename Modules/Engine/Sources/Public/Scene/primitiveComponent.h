#pragma once

#include <EnginePCH.h>
#include "sceneComponent.h"

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
	virtual ~PrimitiveComponent();

	virtual void Tick() override;
	virtual void RebuildTransformData()override;
	virtual void RebuildLocalBounds() { RebuildWorldBounds(); }
	virtual void RebuildWorldBounds();
	BoxSphereBound GetLocalBounds() const { return localBounds; }
	BoxSphereBound GetWorldBounds() const { return worldBounds; }
	virtual void DrawEditorWindow() override;

	static void SetDrawSphereBounds(bool bInDrawDebug);
	static void SetDrawBoxBounds(bool bInDrawDebug);
	static bool DoesDrawSphereBounds();
	static bool DoesDrawBoxBounds();
};