#include "AssetRegistry.h"
#include "assetLibrary.h"
#include "GLAssetIO.h"
#include "../Texture/texture.h"
#include "../Mesh/staticMesh.h"
#include "../Shader/material.h"
#include "../EngineLog/engineLog.h"

std::vector<Asset*> AssetRegistry::registeredAssets;

void AssetRegistry::ImportAssetFromDirectory(std::string RootFolder)
{
	std::vector<std::string> filesToLoad = AssetLibrary::CollectFilesUnderFolder(RootFolder);

	for (auto& filePath : filesToLoad)
	{
		std::string fileType;

		SAssetReader reader(filePath);
		SPropertyValue assetTypeValue(reader.Get(), "AssetType");
		if (assetTypeValue.IsValid())
		{
			std::string assetTypeText = assetTypeValue.GetValue<const char>();
			if (assetTypeText == "Texture2D")
			{
				new Texture2D(filePath);
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
			GLog(LogVerbosity::Error, "AssetRegistry", "failed to read asset type for " + filePath);
		}
	}
}

void AssetRegistry::RegisterAsset(Asset* newAsset)
{
	AssetRegistry::registeredAssets.push_back(newAsset);
}

