#include "asset.h"

#include "rapidjson/document.h"
#include <iostream>
#include <fstream>
#include <string>
#include "assetLibrary.h"
#include "AssetRegistry.h"



Asset::Asset(std::string inAssetPath)
{
	Initialize(inAssetPath);
}

void Asset::Initialize(std::string inAssetPath)
{
	assetPath = inAssetPath;
	SAssetReader assetRead(inAssetPath);
	if (!assetRead.IsValid())
	{
		assetName = AssetLibrary::GenerateNonExistingAssetName("CorruptedAsset");
		std::cout << "ERROR : failed to load asset " << assetName << " ( " << assetPath << " ) " << std::endl;
	}

	AssetRegistry::RegisterAsset(this);
	if (!assetRead.IsValid())
	{
		std::cout << "Error : failed to open path " << inAssetPath << std::endl;
		return;
	}
	SStringPropertyValue* assetNameProperty = new SStringPropertyValue(assetRead.Get(), "AssetName");
	assert(RegisterProperty(assetNameProperty));
	assetName = assetNameProperty->GetStringValue();
	std::cout << "Initialized asset " << assetName << " ( " << assetPath << " ) " << std::endl;
}

bool Asset::ChangeFilePath(std::string inNewPath)
{
	bool bDoesReloadData = bAreDataLoaded;
	if (bDoesReloadData) { UnloadData(); }
	Initialize(inNewPath);
	if (bDoesReloadData) { LoadData(); }
	return true;
}

bool Asset::LoadData()
{
	if (bAreDataLoaded) return false;
	ImportData();
	bAreDataLoaded = true;
	return true;
}

bool Asset::UnloadData()
{
	if (!bAreDataLoaded) return false;
	for (const auto& propertyElem : assetProperties)
	{
		if (propertyElem) delete propertyElem;
	}
	assetProperties.clear();
	bAreDataLoaded = false;
	return true;
}

SPropertyValue* Asset::GetProperty(const std::string propertyName)
{
	LoadData();
	for (const auto& prop : assetProperties)
	{
		if (prop)
		{
			if (prop->GetPropertyName() == propertyName) return prop;
		}
	}
	return nullptr;
}

void Asset::SetProperty(const std::string propertyName, const SPropertyValue& property)
{
	LoadData();
}

void Asset::ImportData()
{
	std::cout << "Imported asset " << assetName << " ( " << assetPath << " ) " << std::endl;
}
