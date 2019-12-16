#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H
#include <string>
#include <vector>

class World;

class EditorWindow
{
private:
	static void DrawMainToolbar(World* InWorld);
	static void DrawBackgroundDockspace(World* InWorld);
public:
	static void DrawWindow(World* InWorld, float& ViewportLocationX, float& ViewportLocationY);
	static double GetMaxFramerate();
};

class UIWindowElement
{
public:
	UIWindowElement(std::string inWindowTitle);
	std::string windowTitle;

	bool bKeepOpen = true;
	bool bCancel = true;
	virtual void Draw(World* inWorld) {}
};

class WindowManager
{
private:
	static std::vector<UIWindowElement*> elementsArray;
public:
	static std::string GetUnexistingWindowName(std::string inWindowName);

	static void DrawElements(World* inWorld);
	static void AddWindow(UIWindowElement* inElement);
	static void CloseWindow(UIWindowElement* inElement);

};
#endif