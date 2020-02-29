
#include "Mesh/MeshComponent.h"
#include "Scene/SceneMeshProxy.h"
#include <Shader/material.h>

void MeshComponent::Render()
{
	PrimitiveComponent::Render();

	if (!bHasProxyBeenCreated && !ProxyMeshData.IsEmpty())
	{
		bHasProxyBeenCreated = true;
		SceneProxy = new SceneMeshProxy(this);
	}

	if (SceneProxy)
	{
		if (ProxyMeshData.HasDataBeenChangedRenderThread())
		{
			ProxyMeshData.CopyDataToSceneMeshProxy(SceneProxy);
		}
		SceneProxy->SetRenderTransform(MeshTransformMatrix);
	}
}

void MeshComponent::SetMaterial(const unsigned int inSectionIndex, Material* newMaterial, const unsigned int& inLodLevel /*= 0*/)
{
	ProxyMeshData.SetMaterial(inSectionIndex, newMaterial, inLodLevel);
}

void MeshComponent::SetMeshData(const unsigned int& LodLevel, const SProxySectionData& inData, const unsigned int& SectionIndex /*= 0*/)
{
	ProxyMeshData.SetMeshData(SectionIndex, inData, SectionIndex);
}

void MeshComponent::RebuildTransformData()
{
	MeshTransformMatrix = glm::mat4(1.f);
	MeshTransformMatrix = glm::translate(MeshTransformMatrix, GetLocation().ToGLVector());

	MeshTransformMatrix = MeshTransformMatrix * glm::mat4_cast(glm::quat((float)GetRotation().w, (float)GetRotation().x, (float)GetRotation().y, (float)GetRotation().z));
	MeshTransformMatrix = glm::scale(MeshTransformMatrix, GetScale3D().ToGLVector());

	PrimitiveComponent::RebuildTransformData();
}