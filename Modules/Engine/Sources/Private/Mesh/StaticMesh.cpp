
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <Mesh/StaticMesh.h>
#include <Camera/camera.h>
#include <Lighting/directionalLight.h>
#include <Mesh/staticMeshComponent.h>
#include <Shader/material.h>
#include <Asset/AssetRegistry.h>
#include <Scene/scene.h>
#include <Texture/texture.h>
#include "Mesh/MeshData.h"


AStaticMesh::AStaticMesh(std::string dataAssetPath)
	: Asset(dataAssetPath) 
{
	BindOnAssetLoaded(this, &AStaticMesh::ComputeBounds);
}


std::vector<MeshSectionData*> AStaticMesh::GetSections()
{
	CheckData(true);
	if (AreDataLoaded())
	{
		return meshSections;
	}
	else
	{
		return {};
	}
}

std::vector<Material*> AStaticMesh::GetMaterials()
{
	CheckData(true);
	if (AreDataLoaded())
	{
		return usedMaterial;
	}
	else
	{
		return {};
	}
}

void AStaticMesh::LoadProperties()
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

		Material* linkedMat = nullptr;

		MeshSectionData* createdSection = new MeshSectionData(
			verticesData->GetValue<Vertex>(), verticesData->GetBufferSize() / sizeof(Vertex),
			indicesData->GetValue<unsigned int>(), indicesData->GetBufferSize() / sizeof(unsigned int),
			linkedMat,
			{}
		);
		createdSection->ComputeBounds();
		meshSections.push_back(createdSection);
	}
	UpdateMaterials();
	Asset::LoadProperties();
}

void AStaticMesh::UpdateMaterials()
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

void AStaticMesh::BuildThumbnail()
{
	CheckData();
	for (auto& material : usedMaterial)
	{
		material->CheckData();
		for (auto& text : material->textures)
		{
			text->CheckData();
		}
	}


	SPropertyValue* thumbnailProperty = GetBaseProperty("thumbnailTexture");
	Scene* thumbnailScene = new AdvancedScene();
	thumbnailScene->InitializeScene();
	thumbnailScene->Draw();
	StaticMeshComponent* comp = new StaticMeshComponent(thumbnailScene, this, {});
	comp->SetRotation(SRotator(90, 0, 0));
	thumbnailScene->Draw();
	comp->RebuildTransformData();
	thumbnailScene->Draw();
	DirectionalLight* dirLight = new DirectionalLight(thumbnailScene);
	dirLight->lightParams.ambiant = glm::vec4(1);
	thumbnailScene->UpdateFramebufferSize(THUMBNAIL_RESOLUTION, THUMBNAIL_RESOLUTION);
	thumbnailScene->GetCamera()->Pitch = 45;
	thumbnailScene->GetCamera()->Yaw = 35;
	thumbnailScene->GetCamera()->updateCameraVectors();
	thumbnailScene->Draw();
	thumbnailScene->Draw();
	thumbnailScene->GetCamera()->SetLocation(comp->GetWorldBounds().GetOrigin() + thumbnailScene->GetCamera()->GetRotation().GetForwardVector() * comp->GetWorldBounds().GetBoundRadius() * -2.5);
	thumbnailScene->Draw();
	thumbnailScene->Draw();
	float* renderTexture = (float*)malloc(THUMBNAIL_RESOLUTION * THUMBNAIL_RESOLUTION * 3 * sizeof(float));
	glBindTexture(GL_TEXTURE_2D, thumbnailScene->GetColorBuffer());
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, renderTexture);
	thumbnailProperty->SetValue<float>(renderTexture, THUMBNAIL_RESOLUTION * THUMBNAIL_RESOLUTION * 3 * sizeof(float));
	free(renderTexture);
	delete thumbnailScene;
}

void AStaticMesh::OnAssetClicked()
{
	//Todo : new StaticMeshEditorWindows(GetName(), this);
}

void AStaticMesh::SaveAsset()
{
	UpdateMaterials();
	Asset::SaveAsset();
}

void AStaticMesh::ComputeBounds()
{
	bool bSet = false;
	SVector3 min;
	SVector3 max;
	for (const auto& section : GetSections())
	{
		if (section->localBounds.GetBoxMin().x < min.x || !bSet) min.x = section->localBounds.GetBoxMin().x;
		if (section->localBounds.GetBoxMin().y < min.y || !bSet) min.y = section->localBounds.GetBoxMin().y;
		if (section->localBounds.GetBoxMin().z < min.z || !bSet) min.z = section->localBounds.GetBoxMin().z;

		if (section->localBounds.GetBoxMax().x > max.x || !bSet) max.x = section->localBounds.GetBoxMax().x;
		if (section->localBounds.GetBoxMax().y > max.y || !bSet) max.y = section->localBounds.GetBoxMax().y;
		if (section->localBounds.GetBoxMax().z > max.z || !bSet) max.z = section->localBounds.GetBoxMax().z;
		bSet = true;
	}
	bounds = BoxSphereBound(min, max);
}

