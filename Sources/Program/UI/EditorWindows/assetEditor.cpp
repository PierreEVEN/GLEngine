#include "assetEditor.h"
#include "../../Asset/asset.h"
#include "../../ImGUI/imgui.h"
#include <iostream>
#include "../../Asset/GLAssetIO.h"
#include "fileExplorer.h"
#include "../../Asset/assetLibrary.h"
#include "../../EngineLog/engineLog.h"
#include "../../Asset/AssetRegistry.h"

AssetEditorWindow::AssetEditorWindow(std::string inWindowName, Asset* inAsset)
 : UIWindowElement(inWindowName)
{
	linkedAsset = inAsset;
	hiddenProperties = { "AssetType" };
}

void AssetEditorWindow::ShowProperty(SPropertyValue* inProperty)
{
	if (!inProperty || !inProperty->IsValid()) return;

	if (std::find(hiddenProperties.begin(), hiddenProperties.end(), inProperty->GetPropertyName()) != hiddenProperties.end()) return;

	if (inProperty->GetPropertyType() == PropertyTypeBool) DisplayBoolProperty((SBoolPropertyValue*)inProperty);
	else if (inProperty->GetPropertyType() == PropertyTypeString) DisplayStringProperty((SStringPropertyValue*)inProperty);
	else if (inProperty->GetPropertyType() == PropertyTypeInt) DisplayIntProperty((SIntPropertyValue*)inProperty);
	else if (inProperty->GetPropertyType() == PropertyTypeUInt) DisplayUIntProperty((SUIntPropertyValue*)inProperty);
	else if (inProperty->GetPropertyType() == PropertyTypeAssetRef) DisplayAssetRefProperty((SAssetRefPropertyValue*)inProperty);
	else if (inProperty->GetPropertyType() == PropertyTypeFileRef) DisplayFileRefProperty((SFileRefPropertyValue*)inProperty);
	else DisplayEmptyProp(inProperty);
}

void AssetEditorWindow::DisplayEmptyProp(SPropertyValue* inProperty)
{
	if (inProperty && inProperty->IsValid()) ImGui::Text(std::string(inProperty->GetPropertyName()).data());
}

void AssetEditorWindow::DisplayBoolProperty(SBoolPropertyValue* inProperty)
{
	bool value = inProperty->GetBoolValue();
	ImGui::Checkbox(inProperty->GetPropertyName().data(), &value);
}

void AssetEditorWindow::DisplayStringProperty(SStringPropertyValue* inProperty)
{
	std::string value = inProperty->GetStringValue();
	char* propertySize = (char*)malloc(100);
	propertySize = (char*)value.data();
	ImGui::InputText(inProperty->GetPropertyName().data(), propertySize, 100);

	if (std::string(propertySize) != value)
	{
		std::string testVal = std::string(propertySize);
		inProperty->SetStringValue(testVal);
	}
}

void AssetEditorWindow::DisplayIntProperty(SIntPropertyValue* inProperty)
{
	int localValue = inProperty->GetIntValue();
	ImGui::SliderInt(inProperty->GetPropertyName().data(), &localValue, -1000, 1000);

	if (localValue != inProperty->GetIntValue())
	{
		inProperty->SetIntValue(localValue);
	}
}

void AssetEditorWindow::DisplayUIntProperty(SUIntPropertyValue* inProperty)
{
	DisplayEmptyProp(inProperty);
}

void AssetEditorWindow::DisplayAssetRefProperty(SAssetRefPropertyValue* inProperty)
{
	if (ImGui::Button(std::string(inProperty->GetPropertyName() + " : " + inProperty->GetAssetRef()).data()))
	{
		currentEditedAssetRef = inProperty;
	}
	if (currentEditedAssetRef == inProperty)
	{
		if (Asset* foundAsset = AssetRegistry::FindAssetByName<Asset>(inProperty->GetAssetRef()))
		{
			for (auto& asset : AssetRegistry::FindAssetsOfType<Asset>(foundAsset->GetAssetType()))
			{
				ImGui::Begin(std::string("Select asset for " + inProperty->GetOwner()->GetName()).data());
				if (ImGui::Button(asset->GetName().data()))
				{
					if (currentEditedAssetRef) currentEditedAssetRef->SetAssetRef(asset->GetName());
					currentEditedAssetRef = nullptr;
				}
				ImGui::End();
			}
		}
		else
		{
			for (auto& asset : AssetRegistry::GetAssets())
			{
				ImGui::Begin(std::string("Select asset for " + inProperty->GetOwner()->GetName()).data());
				if (ImGui::Button(asset->GetName().data()))
				{
					if (currentEditedAssetRef) currentEditedAssetRef->SetAssetRef(asset->GetName());
					currentEditedAssetRef = nullptr;
				}
				ImGui::End();
			}
		}
	}
}

void AssetEditorWindow::DisplayFileRefProperty(SFileRefPropertyValue* inProperty)
{
	if (ImGui::Button(std::string(inProperty->GetPropertyName() + " : " + inProperty->GetFileRef()).data()))
	{
		CurrentEditedFileRef = inProperty;
		currentEditedFileRefNewPath = "";
		GLog(LogVerbosity::Warning, "test", inProperty->GetFileRef());
		GLog(LogVerbosity::Warning, "test", AssetLibrary::GetExtension(inProperty->GetFileRef()));
		new FileExplorer(inProperty->GetPropertyName(), ".", nullptr, &currentEditedFileRefNewPath, { AssetLibrary::GetExtension(inProperty->GetFileRef()) }, AssetLibrary::GetExtension(inProperty->GetFileRef()) == "" ? false : true, false);
	}
	if (inProperty == CurrentEditedFileRef && currentEditedFileRefNewPath != "" && inProperty->GetFileRef() != currentEditedFileRefNewPath)
	{
		inProperty->SetFileRef(currentEditedFileRefNewPath);
		CurrentEditedFileRef = nullptr;
		currentEditedFileRefNewPath = "";
	}
}

void AssetEditorWindow::Draw(World* inWorld)
{
	UIWindowElement::Draw(inWorld);
	ImGui::Begin(std::string("Static mesh editor #" + std::to_string(GetAsset()->GetDynamicID())).data(), &bKeepOpen);
	if (GetAsset()->IsAssetDirty())
	{
		if (ImGui::Button("Save changes"))
		{
			GetAsset()->SaveAsset();
		}
	}
	for (const auto& currentProperty : GetAsset()->GetAssetBaseProperties())
	{
		ShowProperty(currentProperty);
	}
	for (const auto& currentProperty : GetAsset()->GetAssetProperties())
	{
		ShowProperty(currentProperty);
	}
	ImGui::End();

}

StaticMeshEditorWindows::StaticMeshEditorWindows(std::string inWorld, Asset* inAsset)
	: AssetEditorWindow(inWorld, inAsset)
{
	hiddenProperties.push_back("SectionCount");
}

void StaticMeshEditorWindows::ShowProperty(SPropertyValue* inProperty)
{
	if (inProperty->GetPropertyName().find("_Vertices") != std::string::npos) return;
	if (inProperty->GetPropertyName().find("_Indices") != std::string::npos) return;

	AssetEditorWindow::ShowProperty(inProperty);
}
