#include "assetSelector.h"
#include "../../ImGUI/imgui.h"
#include "../../Asset/AssetRegistry.h"
#include "../../Asset/asset.h"

void AssetSelector::Draw(World* inWorld)
{
	if (ImGui::BeginMenu((selectedAsset ? selectedAsset->GetName().data() : label.data())))
	{
		class asset;
		for (Asset* asset : AssetRegistry::GetAssetsByClass<Asset>())
		{
			if (ImGui::Button(asset->GetName().data()))
			{
				selectedAsset = asset;
			}
		}
		ImGui::EndMenu();
	}
}