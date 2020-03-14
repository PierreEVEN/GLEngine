#pragma once

#include <CorePCH.h>

class World;
class ImFloatingWindow;

class EditorWindow
{
private:
	static void DrawMainToolbar(World* InWorld);
	static void DrawBackgroundDockspace(World* InWorld);
public:
	static void DrawWindow(World* InWorld, float& ViewportLocationX, float& ViewportLocationY);
	static double GetMaxFramerate();
};

class WindowManager
{
private:
	static std::vector<ImFloatingWindow*> elementsArray;
public:
	static std::string GetUnexistingWindowName(std::string inWindowName);

	static void DrawElements(World* inWorld);
	static void AddWindow(ImFloatingWindow* inElement);
	static void CloseWindow(ImFloatingWindow* inElement);
};