#pragma once

#include "../EditorWindow.h"

class ContentBrowser : public UIWindowElement
{
public:
	ContentBrowser(std::string inWindowName) : UIWindowElement(inWindowName) {}
	bool bShowMaterials = false;
	bool bShowMesh = true;
	bool bShowTextures = false;
	virtual void Draw(World* InWorld) override;
};