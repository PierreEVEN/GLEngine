
#pragma once

#include "meshSectionComponent.h"

class CubemapComponent : public MeshSectionComponent
{

public:

	CubemapComponent(Scene* inScene, StaticMeshComponent* inParentStaticMeshComponent, MeshSectionData* inStaticMeshSection);

	virtual void Tick() override;

	virtual void RebuildLocalBounds() override {}
};