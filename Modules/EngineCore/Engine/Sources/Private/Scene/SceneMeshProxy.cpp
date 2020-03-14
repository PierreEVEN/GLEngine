#include <Scene/SceneMeshProxy.h>
#include <Mesh/MeshComponent.h>
#include <Shader/material.h>
#include "Scene/scene.h"
#include <Engine/ThreadHandler.h>
#include <OpenglEnginePCH.h>

void SceneMeshProxy::Draw()
{
	for (unsigned int i = 0; i < ProxyData.GetSectionCount(0); ++i)
	{
		Material* usedMaterial = ProxyData.GetMaterial(0, i);
		if (usedMaterial)
		{
			usedMaterial->use();
			usedMaterial->setMat4("model", RenderTransform);
		}
		else
		{
			/** Draw default material */
			MaterialEditorDebuger::GetGridMaterial()->use();
			MaterialEditorDebuger::GetGridMaterial()->setMat4("model", RenderTransform);
		}

		unsigned int vao = ProxyData.GetVao(0, i);
		unsigned int triangles = ProxyData.GetTriangleCount(0, i);

		if (vao >= 0 && triangles > 0)
		{
			/** Draw vertices */
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, triangles, GL_UNSIGNED_INT, 0);

			/** Set GL to defaults */
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}
	}
}

SceneMeshProxy::SceneMeshProxy(MeshComponent* sourceComponent) :
	SceneMeshProxy(sourceComponent->GetScene(), sourceComponent->GetMeshData(), sourceComponent->GetMeshTransform(), sourceComponent->drawPriority) {
}

SceneMeshProxy::SceneMeshProxy(Scene* renderScene, const SProxyLodGroup& inMeshData, glm::mat4 inRenderTransform, const DrawPriority& inDrawLayer) :
	ProxyData(inMeshData),
	LodCount(inMeshData.GetLodCount()),
	RenderTransform(inRenderTransform),
	ProxyDrawLayer(inDrawLayer),
	bIsVisible(true),
	bIsWaitingForDestruction(false)
{
	CHECK_RENDER_THREAD;
	renderScene->RegisterSceneMeshProxy(this);
}

void SceneMeshProxy::DestroySceneProxy()
{
	bIsWaitingForDestruction = true;
}

void SceneMeshProxy::UpdateDataRenderThread(SProxyLodGroup* newData) {
	CHECK_RENDER_THREAD;
	memcpy(&ProxyData, newData, sizeof(SProxyLodGroup));
}

void SceneMeshProxy::SetRenderTransform(const glm::mat4& inTransform) {
	CHECK_RENDER_THREAD;
	RenderTransform = inTransform;
}

void SceneMeshProxy::SetVisibility(const bool& bInVIsible)
{
	CHECK_RENDER_THREAD;
	bIsVisible = bInVIsible;
}