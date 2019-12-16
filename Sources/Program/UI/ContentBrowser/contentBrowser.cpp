#include "contentBrowser.h"
#include "../../ImGUI/imgui.h"
#include "../../Asset/AssetRegistry.h"
#include "../../Shader/material.h"
#include "../EditorWindows/assetEditor.h"
#include "../../Mesh/staticMesh.h"
#include "../../Mesh/staticMeshComponent.h"

void ContentBrowser::Draw(World* InWorld)
{
	ImGui::Begin(windowTitle.data(), &bKeepOpen);
	ImGui::BeginGroup();
	ImGui::Columns(2, 0, true);
	ImGui::SetColumnWidth(0, 150);
	ImGui::Checkbox("show materials ", &bShowMaterials);
	ImGui::Checkbox("show meshes ", &bShowMesh);
	ImGui::Checkbox("show textures ", &bShowTextures);
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
					//ImGui::Button(asset->GetName().data());
				}
			}
			if (bShowTextures)
			{
				if (Texture2D* foundText = dynamic_cast<Texture2D*>(asset))
				{
					asset->DrawContentBrowserIcon();
					ImGui::NextColumn();
					//ImGui::Button(asset->GetName().data());
				}
			}
			if (bShowMesh)
			{
				if (StaticMesh* foundMesh = dynamic_cast<StaticMesh*>(asset))
				{
					asset->DrawContentBrowserIcon();
					ImGui::NextColumn();
					// 				if (ImGui::Button(asset->GetName().data()))
					// 				{
					// 					new StaticMeshEditorWindows("mesh editor", foundMesh);
					// 					StaticMeshComponent* newComp = new StaticMeshComponent(InWorld, foundMesh, {});
					// 					newComp->SetAngle(90.f);
					// 					newComp->SetScale3D(SVector3(10.f));
					// 				}
				}
			}
		}
		ImGui::Columns(1);
	}
	ImGui::EndChild();
	ImGui::Columns(1);
	ImGui::End();
}
