
#pragma once

#include <Scene/primitiveComponent.h>
#include "MeshData.h"

class Scene;
class SceneMeshProxy;

class MeshComponent : public PrimitiveComponent
{
private:

	bool bHasProxyBeenCreated = false;
	SceneMeshProxy* SceneProxy;

protected:

	SProxyLodGroup ProxyMeshData;
	glm::mat4 MeshTransformMatrix;
	
public:

	MeshComponent(Scene* inScene) : PrimitiveComponent(inScene) {}
	virtual ~MeshComponent() {}

	virtual void Render() override;
	void SetMaterial(const unsigned int inSectionIndex, Material* newMaterial, const unsigned int& inLodLevel = 0);
	void SetMeshData(const unsigned int& LodLevel, const SProxySectionData& inData, const unsigned int& SectionIndex = 0);
	virtual void RebuildTransformData() override;
	

	FORCEINLINE SProxyLodGroup GetMeshData() const { return ProxyMeshData; }
	FORCEINLINE glm::mat4 GetMeshTransform() const { return MeshTransformMatrix; }
};