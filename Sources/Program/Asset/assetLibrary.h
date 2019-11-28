#ifndef ASSETLIBRARY_H
#define ASSETLIBRARY_H


#include <vector>
#include <string>
#include <iostream>

class Asset;
class Texture2D;

class AssetLibrary
{
public:


	static std::vector<std::string> CollectFilesUnderFolder(std::string folder);

	static void LoadLibraryFiles(std::string RootFolder);

	static bool CheckExtension(const std::string& filePath, const std::string& extension);

	static std::string GenerateNonExistingAssetName();

	static void RegisterDynamicAsset(Asset* newAsset);

	template <class T>
	static T* FindAssetByName(const std::string AssetName)
	{
		for (Asset* asset : GetAssetRegistry())
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
	static T* LoadAsset(const std::string filePath)
	{
		T* Asset = new T(filePath);
		Asset->LoadAsset();
		return Asset;
	}

	static std::vector<Asset*> GetAssetRegistry();
};


#endif