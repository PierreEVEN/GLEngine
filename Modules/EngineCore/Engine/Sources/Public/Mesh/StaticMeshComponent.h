#pragma once

#include <CorePch.h>
#include "MeshComponent.h"

class AStaticMesh;
class Material;

class StaticMeshComponent : public MeshComponent
{

	bool bHasMeshDataBeenInitialized = false;

public:


	AStaticMesh* meshData;

	StaticMeshComponent(Scene* inScene, AStaticMesh* inMeshData, std::vector<Material*> newMaterialOverride = {});
	virtual ~StaticMeshComponent();

	virtual void Render() override;
};