#pragma once

#include <EnginePCH.h>
#include <Scene/primitiveComponent.h>

class AStaticMesh;
class Material;
class MeshSectionComponent;
class btCollisionShape;
struct btDefaultMotionState;
class btRigidBody;


class StaticMeshComponent : public PrimitiveComponent
{
public:


	AStaticMesh* meshData;

	std::vector<MeshSectionComponent*> sections;
	std::vector<Material*> tmpMaterialOveride;

	void BuildMesh();
	
	StaticMeshComponent(Scene* inScene, AStaticMesh* inMeshData, std::vector<Material*> newMaterialOverride, btCollisionShape* physicBody, bool bSimulatePhysics);
	StaticMeshComponent(Scene* inScene, AStaticMesh* inMeshData, std::vector<Material*> newMaterialOverride = {});
	virtual ~StaticMeshComponent();

	virtual void Tick() override;
	virtual void RebuildTransformData() override;
	virtual void RebuildLocalBounds() override;
	virtual void DrawEditorWindow() override;
};