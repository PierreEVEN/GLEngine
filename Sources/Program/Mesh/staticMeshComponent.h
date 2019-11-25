#ifndef STATICMESHCOMPONENT_H
#define STATICMESHCOMPONENT_H

#include "../World/sceneComponent.h"
#include <vector>

class StaticMesh;
class Material;
class MeshSectionComponent;

class StaticMeshComponent : public SceneComponent
{
public:

	StaticMesh* meshData;

	std::vector<Material*> materialOverride;
	std::vector<MeshSectionComponent*> sections;

	StaticMeshComponent(World* newWorld, StaticMesh* inMeshData, std::vector<Material*> newMaterialOverride = {});

	virtual void SetLocation(glm::vec3 newLocation) override;
	virtual void SetForwardVector(glm::vec3 newForwardVector) override;
	virtual void SetAngle(float newAngle) override;
	virtual void SetScale3D(glm::vec3 newScale3D) override;
};
#endif