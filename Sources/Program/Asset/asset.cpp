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
#include "../UI/EditorWindows/assetEditor.h"

unsigned long AssetCount = 0;

Asset::Asset(std::string inAssetPath)
{
	Initialize(inAssetPath);
	assetDynamicID = AssetCount;
	AssetCount++;
	bIsAssetDirty = false;
}

void Asset::Initialize(std::string inAssetPath)
{
	assetPath = inAssetPath;
	AssetRegistry::RegisterAsset(this);
	SAssetReader assetRead(inAssetPath);
	if (!assetRead.IsValid())
	{
		SStringPropertyValue* assetNameTempProp = new SStringPropertyValue("AssetName");
		std::string tempAssetName = AssetLibrary::GenerateNonExistingAssetName("CorruptedAsset");
		assetNameTempProp->SetStringValue(tempAssetName);
		assert(RegisterBaseProperty(assetNameTempProp));
		GLog(LogVerbosity::Error, "AssetLoading", "failed to load asset " + GetName() + " ( " + GetPath() + " ) ");
		return;
	}

	SStringPropertyValue* assetNameProperty = new SStringPropertyValue(this, assetRead.Get(), "AssetName");
	SStringPropertyValue* assetTypeProperty = new SStringPropertyValue(this, assetRead.Get(), "AssetType");
	assert(RegisterBaseProperty(assetNameProperty));
	assert(RegisterBaseProperty(assetTypeProperty));
	GLog(LogVerbosity::Display, "AssetLoading (" + GetAssetType() + ")", "Initialized asset " + GetName() + " ( " + GetPath() + " ) ");
}

std::string Asset::GetName()
{
	if (SPropertyValue* propName = GetBaseProperty("AssetName"))
	{
		if (SStringPropertyValue* propNameVal = (SStringPropertyValue*)(propName))
		{
			return propNameVal->GetStringValue();
		}
	}
	return "ERROR";
}

std::string Asset::GetAssetType()
{
	if (SPropertyValue* propType = GetBaseProperty("AssetType"))
	{
		if (SStringPropertyValue* propTypeVal = (SStringPropertyValue*)(propType))
		{
			return propTypeVal->GetStringValue();
		}
	}
	return "ERROR";
}

bool Asset::RegisterBaseProperty(SPropertyValue* inNewProperty)
{
	if (inNewProperty && inNewProperty->IsValid())
	{
		assetBaseProperties.push_back(inNewProperty);
		return true;
	}
	return false;
}

bool Asset::RegisterProperty(SPropertyValue* inNewProperty)
{
	if (inNewProperty && inNewProperty->IsValid())
	{
		assetProperties.push_back(inNewProperty);
		return true;
	}
	return false;
}


bool Asset::ChangeFilePath(std::string inNewPath)
{
	bool bDoesReloadData = bAreDataLoaded;
	if (bDoesReloadData) { UnloadData(); }
	Initialize(inNewPath);
	if (bDoesReloadData) { LoadData(); }
	return true;
}

std::vector<SPropertyValue*> Asset::GetAssetBaseProperties()
{
	return assetBaseProperties;
}

std::vector<SPropertyValue*> Asset::GetAssetProperties()
{
	LoadData();
	return assetProperties;
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
	for (const auto& basePropertyElem : assetBaseProperties)
	{
		if (basePropertyElem) delete basePropertyElem;
	}
	for (const auto& propertyElem : assetProperties)
	{
		if (propertyElem) delete propertyElem;
	}
	assetBaseProperties.clear();
	assetProperties.clear();
	bAreDataLoaded = false;
	return true;
}
SPropertyValue* Asset::GetBaseProperty(const std::string propertyName)
{
	for (const auto& prop : GetAssetBaseProperties())
	{
		if (prop)
		{
			if (prop->GetPropertyName() == propertyName) return prop;
		}
	}
	return nullptr;
}


SPropertyValue* Asset::GetProperty(const std::string propertyName)
{
	for (const auto& prop : GetAssetProperties())
	{
		if (prop)
		{
			if (prop->GetPropertyName() == propertyName) return prop;
		}
	}
	return nullptr;
}

void Asset::SaveAsset()
{
	GLAssetIO::SaveAssetProperties(this);
	bIsAssetDirty = false;
}

void Asset::ImportData()
{
	GLog(LogVerbosity::Display, "AssetLoading", "Imported asset " + GetName() + " ( " + assetPath + " ) ");
}



void Asset::OnAssetClicked()
{
	new AssetEditorWindow("AssetEditor", this);
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