#include "importerWindows.h"
#include "../../ImGUI/imgui.h"
#include "../../Asset/AssetImporter.h"
#include "../../Asset/assetLibrary.h"
#include "../EditorWindows/fileExplorer.h"
#include "../../Shader/material.h"
#include "../../Asset/AssetRegistry.h"
#include "../../Texture/texture.h"

void MeshImporterWindow::Draw(World* inWorld)
{
	if (ImGui::Begin("Mesh importer", &bKeepOpen))
	{
		ImGui::Columns(2, "", true);
		if (ImGui::Button("Source file"))
		{
			std::string testString = "invalid value";
			new FileExplorer(".", nullptr, &sourceFilePath, { ".obj", ".fbx" }, true);
		}
		ImGui::NextColumn();

		ImGui::Text(sourceFilePath.data());
		ImGui::NextColumn();
		if (ImGui::Button("output path"))
		{
			new FileExplorer(".", nullptr, &outputFilePath, {}, false, true);
		}
		ImGui::NextColumn();
		ImGui::Text(outputFilePath.data());
		ImGui::NextColumn();
			   
		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::InputText("Asset name", outAssetNameString, sizeof(outAssetNameString));

		if (ImGui::Button("Add material"))
		{
			selectedMaterials.push_back(nullptr);
		}

		for (int i = selectedMaterials.size() - 1; i >= 0; --i)
		{
			if (ImGui::BeginMenu((selectedMaterials[i] ? selectedMaterials[i]->GetName().data() : "None")))
			{
				if (ImGui::Button("remove material"))
				{
					selectedMaterials.erase(selectedMaterials.begin() + i);
					ImGui::EndMenu();
					break;
				}
				for (Material* asset : AssetRegistry::GetAssetsByClass<Material>())
				{
					if (ImGui::Button(asset->GetName().data()))
					{
						selectedMaterials[i] = asset;
					}
				}
				ImGui::EndMenu();
			}
		}

		ImGui::Checkbox("import textures", &bImportTextures);

		ImGui::Separator();
		if (ImGui::Button("confirm"))
		{
			CreateObject();
		}

		ImGui::End();
	}
}

void MeshImporterWindow::CreateObject()
{
	if (sourceFilePath == "") return;
	if (outputFilePath == "") return;
	if (outAssetNameString == "") return;
	std::string uniqueAssetName = AssetLibrary::GenerateNonExistingAssetName(outAssetNameString);
	AssetImporter::ImportMesh(sourceFilePath, uniqueAssetName, outputFilePath + "/" + uniqueAssetName, bImportTextures);
	bKeepOpen = false;
}

void TextureImporterWindow::Draw(World* inWorld)
{
	if (ImGui::Begin("Texture importer", &bKeepOpen))
	{
		ImGui::Columns(2, "", true);
		if (ImGui::Button("Source file"))
		{
			std::string testString = "invalid value";
			new FileExplorer(".", nullptr, &sourceFilePath, { ".png", ".jpg", ".bmp", ".tif" }, true);
		}
		ImGui::NextColumn();
		ImGui::Text(sourceFilePath.data());
		ImGui::NextColumn();

		if (ImGui::Button("output path"))
		{
			new FileExplorer(".", nullptr, &outputFilePath, {}, false, true);
		}
		ImGui::NextColumn();
		ImGui::Text(outputFilePath.data());
		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::InputText("Asset name", outAssetNameString, sizeof(outAssetNameString));
		ImGui::Separator();
		if (ImGui::Button("confirm"))
		{
			CreateObject();
		}
		ImGui::End();
	}
}

void TextureImporterWindow::CreateObject()
{
	if (sourceFilePath == "") return;
	if (outputFilePath == "") return;
	if (outAssetNameString == "") return;
	std::string uniqueAssetName = AssetLibrary::GenerateNonExistingAssetName(outAssetNameString);
	AssetImporter::ImportTexture(sourceFilePath, uniqueAssetName, outputFilePath + "/" + uniqueAssetName);
	bKeepOpen = false;
}

void ShaderImporterWindow::Draw(World* inWorld)
{
	if (ImGui::Begin("Shader importer", &bKeepOpen))
	{
		ImGui::Columns(2, "", true);
		if (ImGui::Button("vertex shader"))
		{
			std::string testString = "invalid value";
			new FileExplorer(".", nullptr, &vertexShadersPath, { ".vs" }, true);
		}
		ImGui::NextColumn();
		ImGui::Text(vertexShadersPath.data());
		ImGui::NextColumn();

		if (ImGui::Button("fragment shader"))
		{
			std::string testString = "invalid value";
			new FileExplorer(".", nullptr, &fragmentShadersPath, { ".fs" }, true);
		}
		ImGui::NextColumn();
		ImGui::Text(fragmentShadersPath.data());
		ImGui::NextColumn();

		if (ImGui::Button("output path"))
		{
			new FileExplorer(".", nullptr, &outputFilePath, {}, false, true);
		}
		ImGui::NextColumn();
		ImGui::Text(outputFilePath.data());
		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::InputText("Asset name", outAssetNameString, sizeof(outAssetNameString));
		if (ImGui::Button("Add texture"))
		{
			selectedTextures.push_back(nullptr);
		}

		for (int i = selectedTextures.size() - 1; i >= 0; --i)
		{
			if (ImGui::BeginMenu((selectedTextures[i] ? selectedTextures[i]->GetName().data() : "None")))
			{
				if (ImGui::Button("remove texture"))
				{
					selectedTextures.erase(selectedTextures.begin() + i);
					ImGui::EndMenu();
					break;
				}
				for (Texture2D* asset : AssetRegistry::GetAssetsByClass<Texture2D>())
				{
					if (asset->HasValidPath())
					{
						if (ImGui::Button(asset->GetName().data()))
						{
							selectedTextures[i] = asset;
						}
					}
				}
				ImGui::EndMenu();
			}
		}
		ImGui::Separator();
		if (ImGui::Button("confirm"))
		{
			CreateObject();
		}
		ImGui::End();
	}
}

void ShaderImporterWindow::CreateObject()
{
	if (vertexShadersPath == "") return;
	if (fragmentShadersPath == "") return;
	if (outAssetNameString == "") return;
	std::vector<std::string> returnTexture;
	for (const auto& texture : selectedTextures)
	{
		if (texture)
		{
			returnTexture.push_back(texture->GetName());
		}
	}
	std::string uniqueAssetName = AssetLibrary::GenerateNonExistingAssetName(outAssetNameString);
	AssetImporter::ImportShader(vertexShadersPath, fragmentShadersPath, uniqueAssetName, outputFilePath + "/" + uniqueAssetName, returnTexture);
	bKeepOpen = false;
}