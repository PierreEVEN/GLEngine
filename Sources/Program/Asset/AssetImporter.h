#ifndef ASSETIMPORTER_H
#define ASSETIMPORTER_H

#include <string>



class AssetImporter
{
public:
	static void GenerateFileBody(const std::string newAssetName, const std::string newFilePath, const char* assetType);

	static void ImportTexture(std::string textureFilePath, std::string newTextureName, std::string newFilePath);
};
#endif