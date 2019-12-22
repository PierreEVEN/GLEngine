#pragma once

#include "../EditorWindow.h"

class Asset;
struct SPropertyValue;
struct SBoolPropertyValue;
struct SStringPropertyValue;
struct SIntPropertyValue;
struct SUIntPropertyValue;
struct SAssetRefPropertyValue;
struct SFileRefPropertyValue;

class AssetEditorWindow : public UIWindowElement
{

private:

	Asset* linkedAsset;

protected:

	std::vector<std::string> hiddenProperties;
public:

	AssetEditorWindow(std::string inWindowName, Asset* inAsset);

	Asset* GetAsset() const { return linkedAsset; }
	virtual void Draw(World* inWorld) override;

	virtual void ShowProperty(SPropertyValue* inProperty);
	void DisplayEmptyProp(SPropertyValue* inProperty);
	void DisplayBoolProperty(SBoolPropertyValue* inProperty);
	void DisplayStringProperty(SStringPropertyValue* inProperty);
	void DisplayIntProperty(SIntPropertyValue* inProperty);
	void DisplayUIntProperty(SUIntPropertyValue* inProperty);
	void DisplayAssetRefProperty(SAssetRefPropertyValue* inProperty);
	void DisplayFileRefProperty(SFileRefPropertyValue* inProperty);
	SAssetRefPropertyValue* currentEditedAssetRef = nullptr;
	SFileRefPropertyValue* CurrentEditedFileRef = nullptr;
	std::string currentEditedFileRefNewPath = "";
};

class StaticMeshEditorWindows : public AssetEditorWindow
{
public:
	StaticMeshEditorWindows(std::string inWorld, Asset* inAsset);

	virtual void ShowProperty(SPropertyValue* inProperty) override;
};