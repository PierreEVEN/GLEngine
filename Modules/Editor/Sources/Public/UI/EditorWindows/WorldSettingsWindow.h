
#pragma once
#include <UI/FloatingWindow.h>

class World;

class WorldSettingsWindow : public ImFloatingWindow
{
private:

	World* parentWorld;

public:

	WorldSettingsWindow(std::string windowTitle, World* inParentWorld);
	virtual void Draw(World* inWorld);
};