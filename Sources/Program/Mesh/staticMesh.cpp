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


StaticMesh::StaticMesh(std::string dataAssetPath)
	: Asset(dataAssetPath) {}

void StaticMesh::LoadMesh(std::string path)
{
	SAssetReader reader(path);
	SPropertyValue* verticeCount = new SPropertyValue(reader.Get(), "SectionCount");
	if (!RegisterProperty(verticeCount)) return;

	unsigned int sectionCount = *verticeCount->GetValue<unsigned int>();
	
	for (unsigned int i = 0; i < sectionCount; ++i)
	{
		SPropertyValue* verticesData = new SPropertyValue(reader.Get(), "Section" + std::to_string(i) + "_Vertices");
		SPropertyValue* indicesData = new SPropertyValue(reader.Get(), "Section" + std::to_string(i) + "_Indices");
		if (!RegisterProperty(verticesData)) continue;
		if (!RegisterProperty(indicesData)) continue;

		std::vector<Vertex> vertices(verticesData->GetValue<Vertex>(), verticesData->GetValue<Vertex>() + verticesData->GetBufferSize() / sizeof(Vertex));
		std::vector<unsigned int> indices(indicesData->GetValue<unsigned int>(), indicesData->GetValue<unsigned int>() + indicesData->GetBufferSize() / sizeof(unsigned int));

		Material* linkedMat = nullptr;

		SStringPropertyValue* linkedMaterialNameData = new SStringPropertyValue(reader.Get(), "Material_" + std::to_string(i));
		if (RegisterProperty(linkedMaterialNameData))
		{
			std::string linkedMaterialName = linkedMaterialNameData->GetStringValue();
			linkedMat = AssetRegistry::FindAssetByName<Material>(linkedMaterialName);
		}

		meshSections.push_back(StaticMeshSection(vertices, indices, linkedMat, {}));
	}
}

void StaticMesh::ImportData()
{
	Asset::ImportData();
	LoadMesh(GetPath());
}
