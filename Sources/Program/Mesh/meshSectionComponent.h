#pragma once

#include "../Physic/primitiveComponent.h"

class World;
struct StaticMeshSection;
class Material;
class StaticMeshComponent;

class MeshSectionComponent : public PrimitiveComponent
{

public:

	MeshSectionComponent(Scene* inScene);
	MeshSectionComponent(Scene* inScene, StaticMeshComponent* inParentStaticMeshComponent, StaticMeshSection* inStaticMeshSection);
	~MeshSectionComponent();
	StaticMeshComponent* parentStaticMeshComponent;
	virtual void Tick() override;
	virtual void RebuildTransformData() override;

	StaticMeshSection* GetSectionData() const { return staticMeshSection; }

	virtual void RebuildLocalBounds() override;
	Material* materialOverride;
protected:
	unsigned int VBO, VAO, EBO;
	glm::mat4 sectionTransform;
private:
	StaticMeshSection* staticMeshSection;
	virtual void BuildMesh();
};

class CubemapSectionComponent : public MeshSectionComponent
{

public:

	CubemapSectionComponent(Scene* inScene, StaticMeshComponent* inParentStaticMeshComponent, StaticMeshSection* inStaticMeshSection);

	virtual void Tick() override;

	virtual void RebuildLocalBounds() override {}
};