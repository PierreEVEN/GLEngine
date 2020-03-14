
#include <Asset/asset.h>
#include <Asset/assetLibrary.h>
#include <Asset/AssetRegistry.h>
#include <Engine/engine.h>

#define LOG_ASSET_LOADING false
#define ENABLE_ASYNC_LOADING true
#define GENERATE_THUMBNAIL_ON_START false

unsigned long AssetCount = 0;

Asset::Asset(std::string inAssetPath)
{
	Initialize(inAssetPath);
	assetDynamicID = AssetCount;
	AssetCount++;
	bIsAssetDirty = false;
	OnAssetLoaded = new Event();
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
	if (bDoesReloadData) { CheckData(); }
	return true;
}

unsigned int Asset::GetAssetThumbnail()
{
	if (thumbnailTextureIndex == -1)
	{
		SPropertyValue* thumbnailProperty = GetBaseProperty("thumbnailTexture");
		if (!thumbnailProperty->IsValueValid() || GENERATE_THUMBNAIL_ON_START)
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
	CheckData();
	return assetProperties;
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
	CheckData();
	BuildThumbnail();
	thumbnailTextureIndex = -1;
	GLAssetIO::SaveAssetProperties(this);
	bIsAssetDirty = false;
}

/************************************************************************/
/* PROPERTY LOADING                                                     */
/************************************************************************/

std::vector<Asset*> AsyncLoadingAssets;

void Asset::ImportAsyncAsset(Asset* inAsset)
{
	inAsset->LoadProperties();
	inAsset->bIsAsyncLoadingProcessComplete = true;
}

void Asset::FlushAsyncLoadingAssets()
{
	if (AsyncLoadingAssets.size() > 0)
	{
		for (int i = (int)AsyncLoadingAssets.size() - 1; i >= 0; --i)
		{
			if (AsyncLoadingAssets[i]->IsPerformingAsyncLoad() && AsyncLoadingAssets[i]->bIsAsyncLoadingProcessComplete)
			{
				delete AsyncLoadingAssets[i]->assetAsyncLoadThread;
				AsyncLoadingAssets[i]->assetAsyncLoadThread = nullptr;
				AsyncLoadingAssets[i]->PostLoadProperties();
				AsyncLoadingAssets.erase(AsyncLoadingAssets.begin() + i);
			}
		}
	}
}

void Asset::BeginPropertyLoad()
{
	if (bIsWaitingForDataLoad || AreDataLoaded()) return;
	bIsWaitingForDataLoad = true;
	bAreDataLoaded = false;
}

void Asset::BeginAsyncPropertyLoad()
{
	if (bIsWaitingForDataLoad || AreDataLoaded()) return;
	bIsWaitingForDataLoad = true;
	bIsAsyncLoadingProcessComplete = false;
	bAreDataLoaded = false;

	AsyncLoadingAssets.push_back(this);
	assetAsyncLoadThread = new std::thread(ImportAsyncAsset, this);
	assetAsyncLoadThread->detach();
}

void Asset::PostLoadProperties()
{
	bIsWaitingForDataLoad = false;
	bIsAsyncLoadingProcessComplete = false;
	bAreDataLoaded = true;
	OnAssetLoaded->execute();
	if (LOG_ASSET_LOADING) GFullLog(LogVerbosity::Display, "Asset", "Load " + GetName());
}

bool Asset::CheckData(bool bLoadAsync)
{
	if (AreDataLoaded()) return true;
	if (bIsWaitingForDataLoad)
	{
		if (!bLoadAsync && assetAsyncLoadThread && Engine::GetGameThreadID() == std::this_thread::get_id())
		{
			while (!bIsAsyncLoadingProcessComplete) {}
		}
 		return false;
	}

	if (bLoadAsync && ENABLE_ASYNC_LOADING)
	{
		BeginAsyncPropertyLoad();
	}
	else
	{
		BeginPropertyLoad();
 		LoadProperties();
 		PostLoadProperties();
		return true;
	}
	return false;
}

bool Asset::AreDataLoaded()
{
  	return bAreDataLoaded && !bIsWaitingForDataLoad && !bIsAsyncLoadingProcessComplete;
}

bool Asset::IsPerformingAsyncLoad() const
{
	return bIsWaitingForDataLoad;
}

/************************************************************************/
/* Editor widgets                                                       */
/************************************************************************/


void Asset::OnAssetClicked()
{
	//@Todo : new AssetEditorWindow("AssetEditor", this);
}


void Asset::DrawContentBrowserIcon()
{
	ImGui::BeginGroup();
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)GetAssetColor());
	if (GetAssetThumbnail() != -1)
	{
#pragma warning( push )
#pragma warning( disable : 4312 )
		if (ImGui::ImageButton((ImTextureID)GetAssetThumbnail(), ImVec2(80, 80), ImVec2(0, 1), ImVec2(1, 0)))
#pragma warning(pop)
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