#pragma once

#include <vector>
#include "../Physic/PrimitiveComponent.h"

class StaticMesh;
class Material;
class MeshSectionComponent;
class btCollisionShape;
struct btDefaultMotionState;
class btRigidBody;


class StaticMeshComponent : public PrimitiveComponent
{
public:


	StaticMesh* meshData;

	std::vector<MeshSectionComponent*> sections;
	std::vector<Material*> tmpMaterialOveride;


	btDefaultMotionState* myMotionState = nullptr;
	btRigidBody* body = nullptr;

	void SetSimulatePhysic(bool bInSimulatePhysics);
	bool GetSimulatePhysics();

	StaticMeshComponent(Scene* inScene, StaticMesh* inMeshData, std::vector<Material*> newMaterialOverride, btCollisionShape* physicBody, bool bSimulatePhysics);
	StaticMeshComponent(Scene* inScene, StaticMesh* inMeshData, std::vector<Material*> newMaterialOverride = {});
	~StaticMeshComponent();
	virtual void SetLocation(SVector3 newLocation) override;
	virtual void SetForwardVector(glm::vec3 newForwardVector) override;
	virtual void SetAngle(float newAngle) override;
	virtual void SetScale3D(SVector3 newScale3D) override;

	virtual SVector3 GetLocation() const override;
	virtual float GetAngle() const override;
	virtual glm::vec3 GetForwardVector() const override;
	virtual SVector3 GetScale3D() const override;

	virtual void Tick() override;
	virtual void RebuildTransformData() override;
	virtual void RebuildLocalBounds() override;
	virtual void DrawEditorWindow() override;
};