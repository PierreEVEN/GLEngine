#pragma once
#include <UI/FloatingWindow.h>


class ContentBrowser : public ImFloatingWindow
{
public:
	ContentBrowser(std::string inWindowName) : ImFloatingWindow(inWindowName) {}
	bool bShowMaterials = false;
	bool bShowMesh = true;
	bool bShowTextures = false;
	bool bShowTextureCubes = false;
	virtual void Draw(World* InWorld) override;
};