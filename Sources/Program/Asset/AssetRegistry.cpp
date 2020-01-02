#include "AssetRegistry.h"
#include "assetLibrary.h"
#include "GLAssetIO.h"
#include "../Texture/texture.h"
#include "../Mesh/staticMesh.h"
#include "../Shader/material.h"
#include "../EngineLog/engineLog.h"
#include "../Engine/debugerTool.h"

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
			GFullLog(LogVerbosity::Warning, "AssetRegistry", "wrong file extension for " + filePath + " (must be .glAsset)");
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
				new StaticMesh(filePath);
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

