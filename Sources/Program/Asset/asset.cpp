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
	if (!assetRead.IsValid()) return;
	SPropertyValue assetNameProperty(assetRead.Get(), "AssetName");
	assert(assetNameProperty.IsValid());
	RegisterProperty(&assetNameProperty);
	assetName = assetNameProperty.GetValue<const char>();
	std::cout << "loaded asset " << assetName << " ( " << assetPath << " ) " << std::endl;
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
			if (prop->propertyName == propertyName) return prop;
		}
	}
	return nullptr;
}

void Asset::SetProperty(const std::string propertyName, const SPropertyValue& property)
{
	LoadData();
}
