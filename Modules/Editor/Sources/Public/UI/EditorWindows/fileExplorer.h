#pragma once

#include <functional>
#include <UI/FloatingWindow.h>
#include <vector>

typedef void(*ConfirmCallback)(std::string path);

struct ImGuiTextFilter;

class FileExplorer : public ImFloatingWindow
{
public:

	FileExplorer(std::string inWindowName, std::string inPath, std::function<void()> inConfirmCallback, std::string* inReturnPath, std::vector<std::string> inExtensions = {}, bool bInCheckExtension = false, bool bInOnlyReturnPath = false);
	
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