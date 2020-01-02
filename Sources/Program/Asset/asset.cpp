#include "asset.h"

#include <iostream>
#include <fstream>
#include <string>
#include "assetLibrary.h"
#include "AssetRegistry.h"
#include "../ImGUI/imgui.h"
#include "../Texture/texture.h"
#include "../EngineLog/engineLog.h"
#include "../UI/EditorWindows/assetEditor.h"
#include <glad/glad.h>
#include <thread>

#define LOG_ASSET_LOADING false

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
		GFullLog(LogVerbosity::Error, "AssetLoading", "failed to load asset " + GetName() + " ( " + GetPath() + " ) ");
		return;
	}

	SStringPropertyValue* assetNameProperty = new SStringPropertyValue(this, assetRead.Get(), "AssetName");
	SStringPropertyValue* assetTypeProperty = new SStringPropertyValue(this, assetRead.Get(), "AssetType");
	SPropertyValue* thumbnailProperty = new SPropertyValue(this, assetRead.Get(), "thumbnailTexture");
	assert(RegisterBaseProperty(assetNameProperty));
	assert(RegisterBaseProperty(assetTypeProperty));
	RegisterBaseProperty(thumbnailProperty);
	if (LOG_ASSET_LOADING) GFullLog(LogVerbosity::Display, "Asset", "Read " + GetAssetType() + " " + GetName());
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
	if (inNewProperty)
	{
		assetBaseProperties.push_back(inNewProperty);
		return true;
	}
	return false;
}

bool Asset::RegisterProperty(SPropertyValue* inNewProperty)
{
	if (inNewProperty)
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

unsigned int Asset::GetAssetThumbnail()
{
	if (thumbnailTextureIndex == -1)
	{
		SPropertyValue* thumbnailProperty = GetBaseProperty("thumbnailTexture");
		if (!thumbnailProperty->IsValueValid())
		{
			BuildThumbnail();
		}
		if (!thumbnailProperty->IsValueValid()) return -1;

		float* data = thumbnailProperty->GetValue<float>();

		glGenTextures(1, &thumbnailTextureIndex);
		glBindTexture(GL_TEXTURE_2D, thumbnailTextureIndex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, THUMBNAIL_RESOLUTION, THUMBNAIL_RESOLUTION, 0, GL_RGB, GL_FLOAT, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load image, create texture and generate mipmaps
		assert(thumbnailTextureIndex != -1);
	}
	return thumbnailTextureIndex;
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

void Asset::ImportData()
{
	bIsLoadingData = false;
}

bool Asset::LoadData(bool bLoadAsync)
{
	if (bIsWaitingForDataAsyncLoad && !bIsLoadingData)
	{
		bIsWaitingForDataAsyncLoad = false;
		OnPropertiesLoaded();
	}
	if (bAreDataLoaded) return false;
	bAreDataLoaded = true;
	bIsLoadingData = true;
	if (bLoadAsync)
	{
		bIsWaitingForDataAsyncLoad = true;
		std::thread v(StartDataImport, this);
		v.detach();
	}
	else
	{
		StartDataImport(this);
		OnPropertiesLoaded();
	}
	return true;
}

bool Asset::UnloadData()
{
	if (!bAreDataLoaded) return false;
	bAreDataLoaded = false;
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
	LoadData();
	BuildThumbnail();
	thumbnailTextureIndex = -1;
	GLAssetIO::SaveAssetProperties(this);
	bIsAssetDirty = false;
}

void Asset::OnPropertiesLoaded()
{
	if (LOG_ASSET_LOADING) GFullLog(LogVerbosity::Display, "Asset", "Load " + GetName());
}



bool Asset::AreDataLoaded()
{
	return !bIsLoadingData && bAreDataLoaded;
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
	if (GetAssetThumbnail() != -1)
	{
		if (ImGui::ImageButton((ImTextureID)GetAssetThumbnail(), ImVec2(80, 80), ImVec2(0, 1), ImVec2(1, 0)))
		{
			OnAssetClicked();
		}
	}
	else
	{
		if (ImGui::Button(("#" + GetName() + (IsAssetDirty() ? "*" : "")).data(), ImVec2(80, 80)))
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

	ImGui::TextWrapped((GetName() + (IsAssetDirty() ? "*" : "")).data());

	ImGui::EndGroup();
}