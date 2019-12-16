#include "asset.h"

#include "rapidjson/document.h"
#include <iostream>
#include <fstream>
#include <string>
#include "assetLibrary.h"
#include "AssetRegistry.h"
#include "../ImGUI/imgui.h"
#include "../Texture/texture.h"
#include "../EngineLog/engineLog.h"

unsigned long AssetCount = 0;

Asset::Asset(std::string inAssetPath)
{
	Initialize(inAssetPath);
	assetDynamicID = AssetCount;
	AssetCount++;
}

void Asset::Initialize(std::string inAssetPath)
{
	assetPath = inAssetPath;
	SAssetReader assetRead(inAssetPath);
	if (!assetRead.IsValid())
	{
		assetName = AssetLibrary::GenerateNonExistingAssetName("CorruptedAsset");
		GLog(LogVerbosity::Error, "AssetLoading", "failed to load asset " + assetName + " ( " + assetPath + " ) ");
	}

	AssetRegistry::RegisterAsset(this);
	if (!assetRead.IsValid())
	{
		GLog(LogVerbosity::Error, "AssetLoading", "failed to open path " + inAssetPath);
		return;
	}
	SStringPropertyValue* assetNameProperty = new SStringPropertyValue(assetRead.Get(), "AssetName");
	assert(RegisterProperty(assetNameProperty));
	assetName = assetNameProperty->GetStringValue();
	GLog(LogVerbosity::Display, "AssetLoading", "Initialized asset " + assetName + " ( " + assetPath + " ) ");
}

bool Asset::ChangeFilePath(std::string inNewPath)
{
	bool bDoesReloadData = bAreDataLoaded;
	if (bDoesReloadData) { UnloadData(); }
	Initialize(inNewPath);
	if (bDoesReloadData) { LoadData(); }
	return true;
}

bool Asset::LoadData()
{
	if (bAreDataLoaded) return false;
	ImportData();
	bAreDataLoaded = true;
	return true;
}

bool Asset::UnloadData()
{
	if (!bAreDataLoaded) return false;
	for (const auto& propertyElem : assetProperties)
	{
		if (propertyElem) delete propertyElem;
	}
	assetProperties.clear();
	bAreDataLoaded = false;
	return true;
}

SPropertyValue* Asset::GetProperty(const std::string propertyName)
{
	LoadData();
	for (const auto& prop : assetProperties)
	{
		if (prop)
		{
			if (prop->GetPropertyName() == propertyName) return prop;
		}
	}
	return nullptr;
}

void Asset::SetProperty(const std::string propertyName, const SPropertyValue& property)
{
	LoadData();
}

void Asset::ImportData()
{
	GLog(LogVerbosity::Display, "AssetLoading", "Imported asset " + assetName + " ( " + assetPath + " ) ");
}



/************************************************************************/
/* Editor widgets                                                       */
/************************************************************************/



void Asset::DrawContentBrowserIcon()
{
	ImGui::BeginGroup();
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)GetAssetColor());
	if (GetAssetThumbnail())
	{
		if (ImGui::ImageButton((ImTextureID)GetAssetThumbnail()->GetTextureID(), ImVec2(80, 80)))
		{
			OnAssetClicked();
		}
	}
	else
	{
		if (ImGui::Button(("#" + GetName()).data(), ImVec2(80, 80)))
		{
			OnAssetClicked();
		}
	}
	ImGui::PopStyleColor(1);
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
 		ImGui::SetDragDropPayload("DDOP_ASSET", &assetDynamicID, sizeof(unsigned long));
		DrawContentBrowserIcon();
		ImGui::EndDragDropSource();
	}

	ImGui::TextWrapped(GetName().data());

	ImGui::EndGroup();
}