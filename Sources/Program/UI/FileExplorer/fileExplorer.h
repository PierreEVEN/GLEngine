#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include "../EditorWindow.h"


class FileExplorer : public UIWindowElement
{
public:

	FileExplorer(std::string inPath);
	
	std::string currentPath;

	virtual void Draw(World* inWorld) override;

	void DrawSubFolderTreeNodeElement(std::string inPath);

};
#endif