#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include "../EditorWindow.h"
#include "../../ImGUI/imgui.h"
#include <functional>

typedef void(*ConfirmCallback)(std::string path);

struct ImGuiTextFilter;

class FileExplorer : public UIWindowElement
{
public:

	FileExplorer(std::string inPath, std::function<void()> inConfirmCallback, std::string* inReturnPath, std::vector<std::string> inExtensions = {}, bool bInCheckExtension = false, bool bInOnlyReturnPath = false);
	
	std::string currentPath;
	std::string selectedPath;
	std::string selectedAsset;
	std::vector<std::string> extensionMask;
	std::string* returnPath;
	bool bCheckExtensions;
	bool bOnlyReturnPath;
	
	std::function<void()> confirmCallback = nullptr;

	virtual void Draw(World* inWorld) override;

	void DrawSubFolderTreeNodeElement(std::string inPath);

};
#endif