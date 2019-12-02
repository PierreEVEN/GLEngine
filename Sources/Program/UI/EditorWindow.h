#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H
#include <string>


class World;

class EditorWindow
{
public:

	static void DrawWindow(World* InWorld);

	static void DrawMainToolbar(World* InWorld);

	static void DrawContentBrowser(std::string directoryPath);

	static void DrawTexture2DImporterWindow();
};
#endif