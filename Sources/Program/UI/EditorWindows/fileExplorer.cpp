#include "fileExplorer.h"
#include "../../ImGUI/imgui.h"
#include <iostream>
#include "../../Asset/assetLibrary.h"

FileExplorer::FileExplorer(std::string inPath, std::function<void()> inConfirmCallback, std::string* inReturnPath, std::vector<std::string> inExtensions /*= {}*/, bool bInCheckExtension /*= false*/, bool bInOnlyReturnPath)
	: UIWindowElement()
{
	currentPath = inPath;
	selectedPath = inPath;
	bCheckExtensions = bInCheckExtension;
	extensionMask = inExtensions;
	confirmCallback = inConfirmCallback;
	returnPath = inReturnPath;
	bOnlyReturnPath = bInOnlyReturnPath;
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
	if (!bKeepOpen) return;

	if (ImGui::Begin("File explorer", &bKeepOpen))
	{

		ImGui::Columns(bOnlyReturnPath ? 1 : 2, "PathFileSeparator", true);
		DrawSubFolderTreeNodeElement(currentPath);
		if (!bOnlyReturnPath)
		{
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
				}
			}
		}
		ImGui::Columns(1);
		ImGui::Separator();
		std::string fileLocalPath = std::string(selectedPath + '/' + selectedAsset);
		ImGui::Columns(2, "OutputFile", false);
		ImGui::SetColumnWidth(0, fileLocalPath.size() * 7.3f);
		ImGui::Text(fileLocalPath.data());
		ImGui::NextColumn();
		if (selectedAsset != "" || (bOnlyReturnPath && selectedPath != ""))
		{
			if (ImGui::Button("Confirm"))
			{
				bCancel = false;
				bKeepOpen = false;
				if (returnPath)
				{
					*returnPath = bOnlyReturnPath ? selectedPath : fileLocalPath;
				}
				if (confirmCallback) confirmCallback();
			}
		}
		ImGui::End();
	}
}

