
#pragma once

#include <CorePCH.h>
#include <Mesh/MeshData.h>

class MeshComponent;
class Scene;

class SceneMeshProxy
{
private:

	bool bIsVisible;
	bool bIsWaitingForDestruction;

	unsigned int LodCount;

	glm::mat4 RenderTransform;
	std::thread* OwningGameThread;
	DrawPriority ProxyDrawLayer;

	SProxyLodGroup ProxyData;

public:

	SceneMeshProxy(Scene* renderScene, const SProxyLodGroup& inMeshData, glm::mat4 inRenderTransform, const DrawPriority& inDrawLayer);

	SceneMeshProxy(MeshComponent* sourceComponent);
	virtual ~SceneMeshProxy() {}

	void Draw();
	void DestroySceneProxy();

	void UpdateDataRenderThread(SProxyLodGroup* newData);

	void SetRenderTransform(const glm::mat4& inTransform);
	void SetVisibility(const bool& bInVIsible);
	
	bool IsWaitingForDestruction() const { return bIsWaitingForDestruction; }
	bool IsVisible() const { return bIsVisible && !bIsWaitingForDestruction; }
	DrawPriority GetDrawPriority() const { return ProxyDrawLayer; }
};