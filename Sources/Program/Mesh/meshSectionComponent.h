#ifndef PRIMITIVECOMPONENT_H
#define PRIMITIVECOMPONENT_H

#include "../World/sceneComponent.h"

class World;
struct StaticMeshSection;

class MeshSectionComponent : public SceneComponent
{

public:

	MeshSectionComponent(World* inWorld);
	MeshSectionComponent(World* inWorld, StaticMeshSection* inStaticMeshSection);
	~MeshSectionComponent();

	virtual void MarkRenderStateDirty() override;

	StaticMeshSection* GetSectionData() const { return staticMeshSection; }

private:
	   
	StaticMeshSection* staticMeshSection;
	unsigned int VBO, VAO, EBO;
	virtual void BuildMesh();
};
#endif