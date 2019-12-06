#ifndef ASSET_H
#define ASSET_H

#include <vector>
#include "GLAssetIO.h"

class Asset
{
private:

	/************************************************************************/
	/* ASSET BASES                                                          */
	/************************************************************************/
	std::string assetPath;
	std::string assetName;
	std::vector<SPropertyValue*> assetProperties;

	void RegisterProperty(SPropertyValue* inNewProperty)
	{
		assetProperties.push_back(inNewProperty);
	}

public:

	void Initialize(const std::string inAssetPath);
	bool ChangeFilePath(const std::string inNewPath);

	/************************************************************************/
	/* ASSET DATAS                                                          */
	/************************************************************************/

private:

	bool bAreDataLoaded;

	bool LoadData();
	bool UnloadData();

public:
	
	SPropertyValue* GetProperty(const std::string propertyName);
	void SetProperty(const std::string propertyName, const SPropertyValue& property);

	virtual void ForceReimport() {}

public:

	Asset(std::string inAssetPath);
		
	std::string GetName() const { return assetName; }
	std::string GetPath() const { return assetPath; }
};
#endif