#pragma once

#include <vector>
#include "../Physic/PrimitiveComponent.h"

class StaticMesh;
class Material;
class MeshSectionComponent;


class StaticMeshComponent : public PrimitiveComponent
{
public:


	StaticMesh* meshData;

	std::vector<Material*> materialOverride;
	std::vector<MeshSectionComponent*> sections;

	StaticMeshComponent(World* newWorld, StaticMesh* inMeshData, std::vector<Material*> newMaterialOverride = {});

	virtual void SetLocation(SVector3 newLocation) override;
	virtual void SetForwardVector(glm::vec3 newForwardVector) override;
	virtual void SetAngle(float newAngle) override;
	virtual void SetScale3D(SVector3 newScale3D) override;
};