#include "fileExplorer.h"
#include "../../ImGUI/imgui.h"
#include <iostream>
#include "../../Asset/assetLibrary.h"

FileExplorer::FileExplorer(std::string inPath, std::vector<std::string> inExtensions /*= {}*/, bool bInCheckExtension /*= false*/)
	: UIWindowElement()
{
	currentPath = inPath;
	selectedPath = inPath;
	bCheckExtensions = bInCheckExtension;
	extensionMask = inExtensions;
}

void FileExplorer::DrawSubFolderTreeNodeElement(std::string inPath)
{
	std::vector<std::string> subFolders = AssetLibrary::GetFolderInPath(inPath);
	for (unsigned int i = 0; i < subFolders.size(); ++i)
	{
		ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (selectedPath == inPath + "/" + subFolders[i])
		{
			node_flag |= ImGuiTreeNodeFlags_Selected;
		}
		if (AssetLibrary::GetFolderInPath(inPath + "/" + subFolders[i]).size() <= 0)
		{
			node_flag |= ImGuiTreeNodeFlags_Leaf;
		}
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flag, subFolders[i].data(), i);
		if (ImGui::IsItemClicked())
		{
			selectedPath = inPath + "/" + subFolders[i];
		}

		if (node_open)
		{
			DrawSubFolderTreeNodeElement(inPath + "/" + subFolders[i]);
			ImGui::TreePop();
		}
	}
}

void FileExplorer::Draw(World* inWorld)
{
	ImGui::ShowDemoWindow();

	ImGui::Begin("File explorer");

	static char buf[32] = "dummy";
	ImGui::InputText("file path", buf, IM_ARRAYSIZE(buf));

	ImGui::Columns(2, "PathFileSeparator", true);
	ImGui::AlignTextToFramePadding();	
	DrawSubFolderTreeNodeElement(currentPath);
	ImGui::NextColumn();
	ImGui::AlignTextToFramePadding();	
	for (const auto& file : AssetLibrary::GetFilesInPath(selectedPath))
	{
		bool IsExtensionValid = false;
		if (!IsExtensionValid)
		{
			for (const auto& extension : extensionMask)
			{
				if (AssetLibrary::CheckExtension(file, extension))
				{
					IsExtensionValid = true;
					break;
				}
			}
		}
		if (IsExtensionValid)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
			if (selectedAsset == file)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}			
			ImGui::TreeNodeEx(file.data(), flags);
			if (ImGui::IsItemClicked())
			{
				selectedAsset = file;
			}
			ImGui::TreePop();
			//ImGui::Button(file.data());
		}
	}

	ImGui::End();
}

