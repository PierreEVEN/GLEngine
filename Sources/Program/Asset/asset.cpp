#include "asset.h"

#include "rapidjson/document.h"
#include <iostream>
#include <fstream>
#include <string>
#include "assetLibrary.h"


Asset::Asset()
{
	assetPath = "";
	assetName = AssetLibrary::GenerateNonExistingAssetName();
}

Asset::Asset(std::string inAssetPath)
{
	assetPath = inAssetPath;
}

void Asset::Parse(const Document& data)
{
	if (!data.IsObject())
	{
		std::cout << "ERROR : Failed to parse asset '" << assetPath << "' (wrong JSon format)" << std::endl;
		assert(data.IsObject());
	}
	assert(data.HasMember("AssetName"));
	assert(data["AssetName"].IsString());
	assetName = data["AssetName"].GetString();

	assert(data.HasMember("AssetName"));
	assert(data["AssetName"].IsString());
	assetName = data["AssetName"].GetString();
}

void Asset::Serialize() { }

void Asset::LoadAsset()
{
	std::string jSon;
	std::string line;
	std::ifstream myfile(assetPath);
	if (myfile.is_open())
	{
		while (std::getline(myfile, line))
		{
			jSon += line += "\n";
		}
		myfile.close();
	}
	else
	{
		std::cout << "FAILED TO LOAD : " << assetPath << std::endl;
		return;
	}
	std::cout << "LOAD : " << assetPath << std::endl;

	Document document;
	document.Parse(jSon.data());
	Parse(document);
}

