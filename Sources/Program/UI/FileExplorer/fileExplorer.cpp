#include "fileExplorer.h"
#include "../../ImGUI/imgui.h"
#include <iostream>
#include "../../Asset/assetLibrary.h"

FileExplorer::FileExplorer(std::string inPath)
	: UIWindowElement()
{
	currentPath = inPath;
}

void FileExplorer::DrawSubFolderTreeNodeElement(std::string inPath)
{
	for (const auto& file : AssetLibrary::GetFolderInPath(inPath))
	{
		if (ImGui::TreeNode(file.data()))
		{
			DrawSubFolderTreeNodeElement(inPath + "/" + file);
			ImGui::TreePop();
		}
	}
}

void FileExplorer::Draw(World* inWorld)
{



	ImGui::ShowDemoWindow();

	ImGui::Begin("File explorer");
	ImGui::Columns(2, "PathFileSeparator", true);  // 3-ways, no border
	ImGui::AlignTextToFramePadding();

	DrawSubFolderTreeNodeElement(currentPath);

	ImGui::NextColumn();
	ImGui::AlignTextToFramePadding();	
	for (const auto& file : AssetLibrary::GetFilesInPath(currentPath))
	{
		ImGui::Text(file.data());
	}

	ImGui::End();


}

