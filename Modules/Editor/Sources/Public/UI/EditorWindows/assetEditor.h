#pragma once

#include <CorePCH.h>
#include <UI/FloatingWindow.h>

class Asset;
struct SPropertyValue;
struct SBoolPropertyValue;
struct SStringPropertyValue;
struct SIntPropertyValue;
struct SUIntPropertyValue;
struct SAssetRefPropertyValue;
struct SFileRefPropertyValue;
class Scene;
class StaticMeshComponent;


class AssetEditorWindow : public ImFloatingWindow
{

private:

	Asset* linkedAsset;

protected:

	std::vector<std::string> hiddenProperties;
	std::vector<std::string> skippedFields;
public:

	AssetEditorWindow(std::string inWindowName, Asset* inAsset);

	Asset* GetAsset() const { return linkedAsset; }
	virtual void Draw(World* inWorld) override;

	virtual void ShowProperty(SPropertyValue* inProperty);
	virtual void DrawHeader() {}
	virtual void DrawFooter() {}
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
	Scene* staticMeshEditorScene;
	StaticMeshComponent* meshComp;
	StaticMeshEditorWindows(std::string inWorld, Asset* inAsset);
	virtual void DrawHeader() override;
};

class MaterialEditorWindow : public AssetEditorWindow
{
public:
	MaterialEditorWindow(std::string inWorld, Asset* inAsset);
	virtual void DrawHeader() override;
	Scene* materialEditorScene;
};

class TextureEditorWindow : public AssetEditorWindow
{
public:
	TextureEditorWindow(std::string inWorld, Asset* inAsset);

};