#pragma once

#include <EnginePCH.h>
#include <Asset/asset.h>
#include "MeshData.h"

class Material;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;
class Texture2D;
class Scene;
struct BoxSphereBound;
struct SLodData;

class AStaticMesh : public Asset
{
private:

	std::vector<Material*> usedMaterial;
	std::vector<MeshSectionData*> meshSections;
	SProxyLodGroup MeshGPUData;
	bool bHasGPUMeshDataBeenCreated = false;

public:

	AStaticMesh(std::string dataAssetPath);

	std::vector<MeshSectionData*> GetSections();
	std::vector<Material*> GetMaterials();
	
	BoxSphereBound bounds;
	void ComputeBounds();

protected:

	virtual void LoadProperties() override;

	virtual void CreateMeshData();

public:

	SProxyLodGroup GetMeshData();

	virtual void UpdateMaterials();

	virtual void BuildThumbnail() override;
	virtual ImColor GetAssetColor() { return ImColor(0.7f, 0.6f, 0.4f, 1.f); }
	virtual void OnAssetClicked() override;
	virtual void SaveAsset() override;
};