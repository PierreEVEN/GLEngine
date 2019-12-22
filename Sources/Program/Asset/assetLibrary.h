#pragma once

#include <vector>
#include <string>
#include <iostream>

class Asset;
class Texture2D;

class AssetLibrary
{
public:

	static std::vector<std::string> GetFilesInPath(std::string path);
	static std::vector<std::string> GetFolderInPath(std::string path, bool returnBack = false);

	static std::vector<std::string> CollectFilesUnderFolder(std::string folder);
	static std::string GetExtension(const std::string& filePath);
	static bool CheckExtension(const std::string& filePath, const std::string& extension);
	static std::string RemoveExtension(const std::string& filePath);
	static std::string GenerateNonExistingAssetName(std::string baseName = "NewAsset");
};