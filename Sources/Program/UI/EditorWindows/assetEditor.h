#ifndef ASSETEDITOR_H
#define ASSETEDITOR_H

#include "../EditorWindow.h"

class Asset;
struct SPropertyValue;
struct SBoolPropertyValue;
struct SStringPropertyValue;
struct SIntPropertyValue;

class AssetEditorWindow : public UIWindowElement
{

private:

	Asset* linkedAsset;
public:

	AssetEditorWindow(Asset* inAsset);

	Asset* GetAsset() const { return linkedAsset; }

	void DisplayProperty(SPropertyValue* inProperty);
	void DisplayEmptyProp(SPropertyValue* inProperty);
	void DisplayBoolProperty(SBoolPropertyValue* inProperty);
	void DisplayStringProperty(SStringPropertyValue* inProperty);
	void DisplayIntProperty(SIntPropertyValue* inProperty);


};

class StaticMeshEditorWindows : public AssetEditorWindow
{

public:

	StaticMeshEditorWindows(Asset* inAsset) : AssetEditorWindow(inAsset) {}

	virtual void Draw(World* inWorld) override;



};







#endif