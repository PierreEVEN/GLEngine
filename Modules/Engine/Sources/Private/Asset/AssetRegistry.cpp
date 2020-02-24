#include <Asset/AssetRegistry.h>
#include <Asset/asset.h>
#include <Asset/assetLibrary.h>
#include <Engine/debugerTool.h>
#include <Texture/texture.h>
#include <Mesh/StaticMesh.h>
#include <Shader/material.h>

#define LOG_ASSET_WARNINGS false

std::vector<Asset*> AssetRegistry::registeredAssets;

void AssetRegistry::ImportAssetFromDirectory(std::string RootFolder)
{
	ProfileStat(std::string("Import stat from " + RootFolder).data());
	std::vector<std::string> filesToLoad = AssetLibrary::CollectFilesUnderFolder(RootFolder);

	for (auto& filePath : filesToLoad)
	{
		std::string fileType;

		SAssetReader reader(filePath);
		SPropertyValue assetTypeValue(nullptr, reader.Get(), "AssetType");
		if (!AssetLibrary::CheckExtension(filePath, ".glAsset"))
		{
			if (LOG_ASSET_WARNINGS) GFullLog(LogVerbosity::Warning, "AssetRegistry", "wrong file extension for " + filePath + " (must be .glAsset)");
		}
		if (assetTypeValue.IsValueValid())
		{
			std::string assetTypeText = assetTypeValue.GetValue<const char>();
			if (assetTypeText == "Texture2D")
			{
				new Texture2D(filePath);
			}
			else if (assetTypeText == "TextureCube")
			{
				new TextureCube(filePath);
			}
			else if (assetTypeText == "StaticMesh")
			{
				new AStaticMesh(filePath);
			}
			else if (assetTypeText == "Material")
			{
				new Material(filePath);
			}
		}
		else
		{
			GFullLog(LogVerbosity::Error, "AssetRegistry", "failed to read asset type for " + filePath);
		}
	}
}

void AssetRegistry::RegisterAsset(Asset* newAsset)
{
	AssetRegistry::registeredAssets.push_back(newAsset);
}

