#include "staticMesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "../Asset/assetLibrary.h"
#include "../Shader/material.h"
#include "../ThirdParty/stb_image.h"
#include <assimp/material.h>
#include "../Asset/AssetRegistry.h"
#include "staticMeshComponent.h"
#include "../UI/EditorWindows/assetEditor.h"
#include "../Engine/debugerTool.h"
#include "../World/scene.h"
#include "../Camera/camera.h"
#include <glad/glad.h>


StaticMesh::StaticMesh(std::string dataAssetPath)
	: Asset(dataAssetPath) {}


std::vector<StaticMeshSection*> StaticMesh::GetSections()
{
	LoadData(true);
	if (AreDataLoaded())
	{
		return meshSections;
	}
	else
	{
		return {};
	}
}

std::vector<Material*> StaticMesh::GetMaterials()
{
	LoadData(true);
	if (AreDataLoaded())
	{
		return usedMaterial;
	}
	else
	{
		return {};
	}
}

void StaticMesh::ImportData()
{
	SAssetReader reader(GetPath());
	SUIntPropertyValue* verticeCount = new SUIntPropertyValue(this, reader.Get(), "SectionCount");
	RegisterProperty(verticeCount);
	unsigned int sectionCount = verticeCount->GetUIntValue();

	for (unsigned int i = 0; i < sectionCount; ++i)
	{
		SPropertyValue* verticesData = new SPropertyValue(this, reader.Get(), "Section" + std::to_string(i) + "_Vertices");
		SPropertyValue* indicesData = new SPropertyValue(this, reader.Get(), "Section" + std::to_string(i) + "_Indices");
		SAssetRefPropertyValue* linkedMaterialNameData = new SAssetRefPropertyValue(this, reader.Get(), "Material_" + std::to_string(i));

		RegisterProperty(verticesData);
		RegisterProperty(indicesData);
		RegisterProperty(linkedMaterialNameData);
	}
	Asset::ImportData();
}

void StaticMesh::OnPropertiesLoaded()
{
	Asset::OnPropertiesLoaded();
	SUIntPropertyValue* verticeCount = (SUIntPropertyValue*)GetProperty("SectionCount");
	unsigned int sectionCount = verticeCount->GetUIntValue();

	for (unsigned int i = 0; i < sectionCount; ++i)
	{
		SPropertyValue* verticesData = GetProperty("Section" + std::to_string(i) + "_Vertices");
		SPropertyValue* indicesData = GetProperty("Section" + std::to_string(i) + "_Indices");
		SAssetRefPropertyValue* linkedMaterialNameData = (SAssetRefPropertyValue*)GetProperty("Material_" + std::to_string(i));

		Material* linkedMat = nullptr;

		StaticMeshSection* createdSection = new StaticMeshSection(
			std::vector<Vertex>(verticesData->GetValue<Vertex>(), verticesData->GetValue<Vertex>() + verticesData->GetBufferSize() / sizeof(Vertex)),
			std::vector<unsigned int>(indicesData->GetValue<unsigned int>(), indicesData->GetValue<unsigned int>() + indicesData->GetBufferSize() / sizeof(unsigned int)),
			linkedMat,
			{}
		);
		meshSections.push_back(createdSection);
	}
	UpdateMaterials();
	ComputeBounds();
}

void StaticMesh::UpdateMaterials()
{
	unsigned int sectionCount = ((SUIntPropertyValue*)GetProperty("SectionCount"))->GetUIntValue();
	for (unsigned int i = 0; i < sectionCount; ++i)
	{
		Material* linkedMat = nullptr;

		if (SPropertyValue* materialProp = GetProperty("Material_" + std::to_string(i)))
		{
			if (materialProp->IsValueValid())
			{
				linkedMat = AssetRegistry::FindAssetByName<Material>(((SAssetRefPropertyValue*)materialProp)->GetAssetRef());
			}
		}
		meshSections[i]->material = linkedMat;
	}
}

void StaticMesh::BuildThumbnail()
{
	SPropertyValue* thumbnailProperty = GetBaseProperty("thumbnailTexture");
	Scene* thumbnailScene = new Scene();
	StaticMeshComponent* comp = new StaticMeshComponent(thumbnailScene, this, {});
	comp->SetAngle(90.f);
	comp->RebuildTransformData();
	DirectionalLight* dirLight = new DirectionalLight(thumbnailScene);
	dirLight->lightParams.ambiant = glm::vec4(1);
	thumbnailScene->UpdateFramebufferSize(THUMBNAIL_RESOLUTION, THUMBNAIL_RESOLUTION);
	thumbnailScene->GetCamera()->Pitch = -45;
	thumbnailScene->GetCamera()->Yaw = 35;
	thumbnailScene->GetCamera()->updateCameraVectors();
	thumbnailScene->GetCamera()->SetCameraLocation(comp->GetWorldBounds().GetOrigin() + thumbnailScene->GetCamera()->Front * comp->GetWorldBounds().GetBoundRadius() * -2.5);
	thumbnailScene->Draw();
	float* renderTexture = (float*)malloc(THUMBNAIL_RESOLUTION * THUMBNAIL_RESOLUTION * 3 * sizeof(float));
	glBindTexture(GL_TEXTURE_2D, thumbnailScene->GetColorBuffer());
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, renderTexture);
	thumbnailProperty->SetValue<float>(renderTexture, THUMBNAIL_RESOLUTION * THUMBNAIL_RESOLUTION * 3 * sizeof(float));
	free(renderTexture);
	delete thumbnailScene;
}

void StaticMesh::OnAssetClicked()
{
	new StaticMeshEditorWindows(GetName(), this);
}

void StaticMesh::SaveAsset()
{
	UpdateMaterials();
	Asset::SaveAsset();
}

void StaticMesh::ComputeBounds()
{
	bool bSet = false;
	SVector3 min;
	SVector3 max;
	for (const auto& section : meshSections)
	{
		if (section->bounds.GetBoxMin().x < min.x || !bSet) min.x = section->bounds.GetBoxMin().x;
		if (section->bounds.GetBoxMin().y < min.y || !bSet) min.y = section->bounds.GetBoxMin().y;
		if (section->bounds.GetBoxMin().z < min.z || !bSet) min.z = section->bounds.GetBoxMin().z;

		if (section->bounds.GetBoxMax().x > max.x || !bSet) max.x = section->bounds.GetBoxMax().x;
		if (section->bounds.GetBoxMax().y > max.y || !bSet) max.y = section->bounds.GetBoxMax().y;
		if (section->bounds.GetBoxMax().z > max.z || !bSet) max.z = section->bounds.GetBoxMax().z;
		bSet = true;
	}
	bounds = BoxSphereBound(min, max);
}

void StaticMeshSection::ComputeBounds()
{
	SVector3 min;
	SVector3 max;
	bool bSet = false;
	for (const auto& vertice : sectionVertices)
	{
		if (vertice.Position.x > max.x || !bSet) max.x = vertice.Position.x;
		if (vertice.Position.y > max.y || !bSet) max.y = vertice.Position.y;
		if (vertice.Position.z > max.z || !bSet) max.z = vertice.Position.z;

		if (vertice.Position.x < min.x || !bSet) min.x = vertice.Position.x;
		if (vertice.Position.y < min.y || !bSet) min.y = vertice.Position.y;
		if (vertice.Position.z < min.z || !bSet) min.z = vertice.Position.z;
		bSet = true;
	}
	bounds = BoxSphereBound(min, max);
}
