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
	ImGui::Checkbox("show materials ", &bShowMaterials);
	ImGui::Checkbox("show meshes ", &bShowMesh);
	ImGui::Checkbox("show textures ", &bShowTextures);

	for (auto& asset : AssetRegistry::GetAssets())
	{
		if (bShowMaterials)
		{
			if (Material* foundMat = dynamic_cast<Material*>(asset))
			{
				ImGui::Button(asset->GetName().data());
			}
		}
		if (bShowTextures)
		{
			if (Texture2D* foundText = dynamic_cast<Texture2D*>(asset))
			{
				ImGui::Button(asset->GetName().data());
			}
		}
		if (bShowMesh)
		{
			if (StaticMesh* foundMesh = dynamic_cast<StaticMesh*>(asset))
			{
				if (ImGui::Button(asset->GetName().data()))
				{
					new StaticMeshEditorWindows("mesh editor", foundMesh);
					StaticMeshComponent* newComp = new StaticMeshComponent(InWorld, foundMesh, {});
					newComp->SetAngle(90.f);
					newComp->SetScale3D(SVector3(10.f));
				}
			}
		}
	}
	ImGui::EndGroup();
	ImGui::End();
}
