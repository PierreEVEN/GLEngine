#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include "../EditorWindow.h"
#include "../../ImGUI/imgui.h"

struct ImGuiTextFilter;

class FileExplorer : public UIWindowElement
{
public:

	FileExplorer(std::string inPath, std::vector<std::string> inExtensions = {}, bool bInCheckExtension = false);
	
	std::string currentPath;
	std::string selectedPath;
	std::string selectedAsset;
	std::vector<std::string> extensionMask;
	bool bCheckExtensions;


	virtual void Draw(World* inWorld) override;

	void DrawSubFolderTreeNodeElement(std::string inPath);

};
#endif