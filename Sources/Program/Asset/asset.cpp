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
	assert(data.IsObject());
	assert(data.HasMember("AssetName"));
	assert(data["AssetName"].IsString());
	assetName = data["AssetName"].GetString();

	assert(data.IsObject());
	assert(data.HasMember("AssetName"));
	assert(data["AssetName"].IsString());
	assetName = data["AssetName"].GetString();
	std::cout << "Parse base : " << std::endl;
}

void Asset::Serialize()
{

}

void Asset::LoadAsset()
{
	std::cout << "LOAD : " << assetPath << std::endl;
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

	Document document;
	document.Parse(jSon.data());
	Parse(document);
}

