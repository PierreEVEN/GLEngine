
#pragma once

#include "MeshComponent.h"

class CubemapComponent : public MeshComponent
{
private:

	void CreateMesh();

	bool bHasMeshDataBeenInitialized = false;

public:

	CubemapComponent(Scene* inScene);

	virtual void Render() override;

	virtual void RebuildLocalBounds() override {}
};