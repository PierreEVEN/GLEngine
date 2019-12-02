#include <iostream>
#include <fstream>  
#include "AssetImporter.h"
#include <assert.h>
#include "../ThirdParty/stb_image.h"
#include "assetSerializer.h"


void AssetImporter::GenerateFileBody(const std::string newAssetName, const std::string newFilePath, const char* assetType)
{
	std::ofstream outfile(newFilePath);
	if (outfile.is_open())
	{
		outfile << "####### GLEngineAsset : " << assetType << " #######" << std::endl;
		outfile << "/* Asset body */" << std::endl;
		outfile << "#assetName = " << newAssetName << std::endl;
		outfile << std::endl;
		outfile << "/* Asset properties */" << std::endl;
		outfile.close();
	}
	else {
		std::cout << "FAILED to find path " << newFilePath << std::endl;
	}
}

void AssetImporter::ImportTexture(std::string textureFilePath, std::string newTextureName, std::string newFilePath)
{
	int x, y, nbrChannels;
	if (stbi_uc *data = stbi_load(textureFilePath.data(), &x, &y, &nbrChannels, 0))
	{
		std::cout << "import texture" << textureFilePath << std::endl;
		GenerateFileBody(newTextureName, newFilePath, "Texture2D");
		AssetSerializer::WriteField(newFilePath, "SizeWidth", std::to_string(x));
		AssetSerializer::WriteField(newFilePath, "SizeHeight", std::to_string(y));
		AssetSerializer::WriteField(newFilePath, "ChannelCount", std::to_string(nbrChannels));
		AssetSerializer::WriteField(newFilePath, "TextureData", (char*)data);
	}
}