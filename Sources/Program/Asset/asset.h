#pragma once

#include <vector>
#include "GLAssetIO.h"
#include "../ImGUI/imgui.h"

#define THUMBNAIL_RESOLUTION 80

class Texture2D;
struct SPropertyValue;

class Asset
{
private:

	/************************************************************************/
	/* ASSET PROPERTIES MANAGEMENT                                          */
	/************************************************************************/
	std::string assetPath;
	/** Each asset has an unique id (could be different after restart) */
	unsigned long assetDynamicID;
	std::vector<SPropertyValue*> assetBaseProperties;
	std::vector<SPropertyValue*> assetProperties;

public:

	bool bIsAssetDirty;

private:

	bool RegisterBaseProperty(SPropertyValue* inNewProperty);

protected:

	bool RegisterProperty(SPropertyValue* inNewProperty);

public:

	/** Each property has to be registered in the asset when it is created */
	std::vector<SPropertyValue*> GetAssetBaseProperties();
	std::vector<SPropertyValue*> GetAssetProperties();
	SPropertyValue* GetBaseProperty(const std::string propertyName);
	SPropertyValue* GetProperty(const std::string propertyName);

	/** An asset is set as dirty when a property has been changed and need to be saved */
	void MarkAssetDirty() { bIsAssetDirty = true; }
	bool IsAssetDirty() const { return bIsAssetDirty; }

	virtual void SaveAsset();

	/************************************************************************/
	/* ASSET DATAS                                                          */
	/************************************************************************/

protected:

	bool bAreDataLoaded = false;
	bool bIsLoadingData = false;
	bool bIsWaitingForDataAsyncLoad = false;;
	bool UnloadData();

private:

	static void StartDataImport(Asset* inAsset)
	{
		inAsset->ImportData();
	}

public:
	bool LoadData(bool bLoadAsync = false);

	void Initialize(const std::string inAssetPath);
	virtual void ImportData();
	virtual void OnPropertiesLoaded();

	bool AreDataLoaded();

public:

	Asset(std::string inAssetPath);

	std::string GetName();
	std::string GetAssetType();
	std::string GetPath() const { return assetPath; }
	bool ChangeFilePath(const std::string inNewPath);
	unsigned long GetDynamicID() const { return assetDynamicID; }
	bool HasValidPath() const { return assetPath != ""; }

	/************************************************************************/
	/* Editor widgets                                                       */
	/************************************************************************/

	virtual void BuildThumbnail() {}
	unsigned int thumbnailTextureIndex = -1;
	virtual unsigned int GetAssetThumbnail();
	virtual ImColor GetAssetColor() { return ImColor(0.5, 0.5, 0.5, 1.f); }
	virtual void OnAssetClicked();

	virtual void DrawContentBrowserIcon();
};