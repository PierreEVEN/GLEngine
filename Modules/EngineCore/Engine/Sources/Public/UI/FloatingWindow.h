#pragma once

#include <string>

class World;

class ImFloatingWindow
{
public:
	ImFloatingWindow(std::string inWindowTitle);
	std::string windowTitle;

	bool bKeepOpen = true;
	bool bCancel = true;
	virtual void Draw(World* inWorld) {}
};