#include "staticMesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "../Asset/assetLibrary.h"
#include "../Shader/material.h"
#include "../ThirdParty/stb_image.h"
#include <assimp/material.h>


StaticMesh::StaticMesh(std::string dataAssetPath)
	: Asset(dataAssetPath)
{
	LoadData(dataAssetPath);
}

void StaticMesh::LoadData(std::string path)
{
	SAssetReader reader(path);
	SPropertyValue verticeCount(reader.Get(), "SectionCount");
	if (!verticeCount.IsValid()) return;
	unsigned int sectionCount = *verticeCount.GetValue<unsigned int>();

	for (unsigned int i = 0; i < sectionCount; ++i)
	{
		SPropertyValue verticesData(reader.Get(), "Section" + std::to_string(i) + "_Vertices");
		SPropertyValue indicesData(reader.Get(), "Section" + std::to_string(i) + "_Indices");
		if (!verticesData.IsValid()) continue;
		if (!indicesData.IsValid()) continue;

		std::vector<Vertex> vertices(verticesData.GetValue<Vertex>(), verticesData.GetValue<Vertex>() + verticesData.bufferSize / sizeof(Vertex));
		std::vector<unsigned int> indices(indicesData.GetValue<unsigned int>(), indicesData.GetValue<unsigned int>() + indicesData.bufferSize / sizeof(unsigned int));


		Material* testMat = new Material("");
		testMat->InitializeShader("Sources/Shaders/Default/defaultGridVertexShader.vs", "Sources/Shaders/Default/defaultGridFragmentShader.fs", {});
		meshSections.push_back(StaticMeshSection(vertices, indices, testMat, {}));
	}
}