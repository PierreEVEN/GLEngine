#ifndef ASSETLIBRARY_H
#define ASSETLIBRARY_H


#include <vector>
#include <string>
#include <iostream>

class Asset;

class AssetLibrary
{
public:


	static std::vector<std::string> CollectFilesUnderFolder(std::string folder);

	static void LoadLibraryFiles(std::string RootFolder);

	static bool CheckExtension(const std::string& filePath, const std::string& extension);

	static std::string GenerateNonExistingAssetName() { return "NewAsset"; }

	static Asset* FindAssetByName(std::string AssetName);
};


#endif