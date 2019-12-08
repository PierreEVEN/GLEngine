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

protected:

	bool RegisterProperty(SPropertyValue* inNewProperty)
	{
		if (inNewProperty && inNewProperty->IsValid())
		{
			assetProperties.push_back(inNewProperty);
			return true;
		}
		return false;
	}

public:

	void Initialize(const std::string inAssetPath);
	bool ChangeFilePath(const std::string inNewPath);

	/************************************************************************/
	/* ASSET DATAS                                                          */
	/************************************************************************/

protected:

	bool bAreDataLoaded;

	bool LoadData();
	bool UnloadData();

public:
	
	SPropertyValue* GetProperty(const std::string propertyName);
	void SetProperty(const std::string propertyName, const SPropertyValue& property);
	std::vector<SPropertyValue*> GetAssetProperties() 
	{
		LoadData();
		return assetProperties; 
	}

	virtual void ImportData();

public:

	Asset(std::string inAssetPath);
		
	std::string GetName() const { return assetName; }
	std::string GetPath() const { return assetPath; }
	bool HasValidPath() const { return assetPath != ""; }
};
#endif