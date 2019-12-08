#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H
#include <string>
#include <vector>

class World;

class EditorWindow
{

private:

	static void DrawMainToolbar(World* InWorld);



public:

	static void DrawWindow(World* InWorld);

	static double GetMaxFramerate();
};

class UIWindowElement
{

public:

	UIWindowElement();

	bool bKeepOpen = true;
	bool bCancel = true;

	virtual void Draw(World* inWorld) {}
};

class WindowManager
{
private:

	static std::vector<UIWindowElement*> elementsArray;

public:

	static void DrawElements(World* inWorld);
	static void AddWindow(UIWindowElement* inElement);
	static void CloseWindow(UIWindowElement* inElement);
};

#endif