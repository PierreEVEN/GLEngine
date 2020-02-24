#include <imgui.h>
#include <Shader/material.h>
#include <Asset/AssetRegistry.h>
#include <Mesh/StaticMesh.h>
#include <UI/EditorWindows/contentBrowser.h>
#include <Texture/texture.h>

void ContentBrowser::Draw(World* InWorld)
{
	ImGui::Begin(windowTitle.data(), &bKeepOpen);
	ImGui::BeginGroup();
	ImGui::Columns(2, 0, true);
	ImGui::SetColumnWidth(0, 150);
	ImGui::Checkbox("show materials ", &bShowMaterials);
	ImGui::Checkbox("show meshes ", &bShowMesh);
	ImGui::Checkbox("show textures ", &bShowTextures);
	ImGui::Checkbox("show cubemaps ", &bShowTextureCubes);
	ImGui::EndGroup();
	ImGui::NextColumn();
	ImGui::BeginChild((windowTitle + "_child").data());
	{
		int widthItems = ((int)ImGui::CalcItemWidth() / 64);

		ImGui::Columns(widthItems < 1 ? 1 : widthItems, 0, false);
		for (auto& asset : AssetRegistry::GetAssets())
		{

			if (bShowMaterials)
			{
				if (Material* foundMat = dynamic_cast<Material*>(asset))
				{
					asset->DrawContentBrowserIcon();
					ImGui::NextColumn();
				}
			}
			if (bShowTextures)
			{
				if (Texture2D* foundText = dynamic_cast<Texture2D*>(asset))
				{
					asset->DrawContentBrowserIcon();
					ImGui::NextColumn();
				}
			}			
			if (bShowTextureCubes)
			{
				if (TextureCube* foundText = dynamic_cast<TextureCube*>(asset))
				{
					asset->DrawContentBrowserIcon();
					ImGui::NextColumn();
				}
			}
			if (bShowMesh)
			{
				if (AStaticMesh* foundMesh = dynamic_cast<AStaticMesh*>(asset))
				{
					asset->DrawContentBrowserIcon();
					ImGui::NextColumn();
				}
			}
		}
		ImGui::Columns(1);
	}
	ImGui::EndChild();
	ImGui::Columns(1);
	ImGui::End();
}
