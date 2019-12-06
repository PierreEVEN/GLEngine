#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H
#include <string>
#include <vector>

class World;

class EditorWindow
{

private:

	static void DrawMainToolbar(World* InWorld);

	static void DrawContentBrowser(std::string directoryPath);

	static void DrawTexture2DImporterWindow();

public:

	static void DrawWindow(World* InWorld);


};

class UIWindowElement
{

public:

	UIWindowElement();

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