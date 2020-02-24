#pragma once

#include <EnginePCH.h>
#include <Scene/primitiveComponent.h>

class World;
struct MeshSectionData;
class Material;
class StaticMeshComponent;

class MeshSectionComponent : public PrimitiveComponent
{

public:

	MeshSectionComponent(Scene* inScene);
	MeshSectionComponent(Scene* inScene, StaticMeshComponent* inParentStaticMeshComponent, MeshSectionData* inStaticMeshSection);
	virtual ~MeshSectionComponent();
	StaticMeshComponent* parentStaticMeshComponent;
	virtual void Tick() override;
	virtual void RebuildTransformData() override;

	MeshSectionData* GetSectionData() const { return staticMeshSection; }

	virtual void RebuildLocalBounds() override;
	Material* materialOverride;
protected:
	unsigned int VBO, VAO, EBO;
	glm::mat4 sectionTransform;
protected:
	MeshSectionData* staticMeshSection;
	virtual void BuildMesh();
};