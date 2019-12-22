#pragma once

#include <vector>
#include "GLAssetIO.h"
#include "../ImGUI/imgui.h"

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

	void SaveAsset();

	/************************************************************************/
	/* ASSET DATAS                                                          */
	/************************************************************************/

protected:

	bool bAreDataLoaded;
	bool LoadData();
	bool UnloadData();

public:

	void Initialize(const std::string inAssetPath);
	virtual void ImportData();

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

	virtual Texture2D* GetAssetThumbnail() { return nullptr; }
	virtual ImColor GetAssetColor() { return ImColor(0.5, 0.5, 0.5, 1.f); }
	virtual void OnAssetClicked();

	virtual void DrawContentBrowserIcon();
};