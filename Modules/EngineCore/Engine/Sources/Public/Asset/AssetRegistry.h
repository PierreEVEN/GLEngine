#pragma once

#include <CorePCH.h>

class Asset;
class AssetRegistry;

class AssetRegistry
{
private:

	static std::vector<Asset*> registeredAssets;

public:
	template <class T>
	static std::vector<T*> GetAssetsByClass()
	{
		std::vector<T*> outArray;
		for (Asset* asset : GetAssets())
		{
			if (T* castedAsset = dynamic_cast<T*>(asset))
			{
				outArray.push_back(castedAsset);
			}
		}
		return outArray;
	}

	template <class T>
	static T* FindAssetByName(const std::string AssetName)
	{
		for (Asset* asset : GetAssets())
		{
			if (asset->GetName() == AssetName)
			{
				if (T* castedAsset = dynamic_cast<T*>(asset))
				{
					return castedAsset;
				}
			}
		}
		return nullptr;
	}

	template <class T>
	static T* FindAssetByID(unsigned long inID)
	{
		for (Asset* asset : GetAssets())
		{
			if (asset->GetDynamicID() == inID)
			{
				if (T* castedAsset = dynamic_cast<T*>(asset))
				{
					return castedAsset;
				}
			}
		}
		return nullptr;
	}

	template <class T>
	static std::vector<T*> FindAssetsOfType(std::string assetType)
	{
		std::vector<T*> outArray = {};
		for (Asset* asset : GetAssets())
		{
			if (asset->GetAssetType() == assetType)
			{
				if (T* castedAsset = dynamic_cast<T*>(asset))
				{
					outArray.push_back(castedAsset);
				}
			}
		}
		return outArray;
	}

	static void ImportAssetFromDirectory(std::string RootFolder);
	static void RegisterAsset(Asset* newAsset);

	static std::vector<Asset*> GetAssets() { return AssetRegistry::registeredAssets; }
};