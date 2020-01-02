#include "assetEditor.h"
#include "../../Asset/asset.h"
#include "../../ImGUI/imgui.h"
#include <iostream>
#include "../../Asset/GLAssetIO.h"
#include "fileExplorer.h"
#include "../../Asset/assetLibrary.h"
#include "../../EngineLog/engineLog.h"
#include "../../Asset/AssetRegistry.h"
#include "../../Shader/material.h"
#include "../../World/scene.h"
#include "../../Mesh/staticMeshComponent.h"
#include "../../Mesh/staticMesh.h"
#include "../../Camera/camera.h"
#include <glfw3/glfw3.h>

AssetEditorWindow::AssetEditorWindow(std::string inWindowName, Asset* inAsset)
 : UIWindowElement(inWindowName)
{
	linkedAsset = inAsset;
	hiddenProperties = { "AssetType" };
}

void AssetEditorWindow::ShowProperty(SPropertyValue* inProperty)
{
	if (!inProperty) return;

	if (std::find(hiddenProperties.begin(), hiddenProperties.end(), inProperty->GetPropertyName()) != hiddenProperties.end()) return;
	for (const auto& prop : skippedFields)
	{
		if (inProperty->GetPropertyName().find(prop) != std::string::npos) return;
	}

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
	if (inProperty) ImGui::Text(std::string(inProperty->GetPropertyName()).data());
}

void AssetEditorWindow::DisplayBoolProperty(SBoolPropertyValue* inProperty)
{
	bool value = inProperty->GetBoolValue();
	ImGui::Checkbox(inProperty->GetPropertyName().data(), &value);
}

void AssetEditorWindow::DisplayStringProperty(SStringPropertyValue* inProperty)
{
	std::string value = inProperty->IsValueValid() ? inProperty->GetStringValue() : "";
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
	int localValue = inProperty->IsValueValid() ? inProperty->GetIntValue() : 0;
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
	if (ImGui::Button(std::string("x : " + inProperty->GetPropertyName()).data(), ImVec2(20, 20)))
	{
		inProperty->SetAssetRef("");
	}
	ImGui::SameLine(30.f);
	if (ImGui::Button(std::string(inProperty->GetPropertyName() + " : " + (inProperty->IsValueValid() ? inProperty->GetAssetRef() : "none")).data()))
	{
		currentEditedAssetRef = inProperty;
	}
	if (currentEditedAssetRef == inProperty)
	{
		Asset* foundAsset = nullptr;
		if (inProperty->IsValueValid())
		{
			foundAsset = AssetRegistry::FindAssetByName<Asset>(inProperty->GetAssetRef());
		}
		if (foundAsset)
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
	if (ImGui::Button(std::string(inProperty->GetPropertyName() + " : " + (inProperty->IsValueValid() ? inProperty->GetFileRef() : "None")).data()))
	{
		CurrentEditedFileRef = inProperty;
		currentEditedFileRefNewPath = "";
		GFullLog(LogVerbosity::Warning, "test", inProperty->GetFileRef());
		GFullLog(LogVerbosity::Warning, "test", AssetLibrary::GetExtension(inProperty->GetFileRef()));
		new FileExplorer(inProperty->GetPropertyName(), ".", nullptr, &currentEditedFileRefNewPath, { AssetLibrary::GetExtension(inProperty->GetFileRef()) }, AssetLibrary::GetExtension(inProperty->GetFileRef()) == "" ? false : true, false);
	}
	if (inProperty == CurrentEditedFileRef && currentEditedFileRefNewPath != "" && (!inProperty->IsValueValid() || inProperty->GetFileRef() != currentEditedFileRefNewPath))
	{
		inProperty->SetFileRef(currentEditedFileRefNewPath);
		CurrentEditedFileRef = nullptr;
		currentEditedFileRefNewPath = "";
	}
}

void AssetEditorWindow::Draw(World* inWorld)
{
	UIWindowElement::Draw(inWorld);
	ImGui::Begin(std::string(windowTitle + " Editor").data(), &bKeepOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
	if (GetAsset()->IsAssetDirty())
	{
		if (ImGui::Button("Save changes"))
		{
			GetAsset()->SaveAsset();
		}
	}
	DrawHeader();
	for (const auto& currentProperty : GetAsset()->GetAssetBaseProperties())
	{
		ShowProperty(currentProperty);
	}
	for (const auto& currentProperty : GetAsset()->GetAssetProperties())
	{
		ShowProperty(currentProperty);
	}
	DrawFooter();
	ImGui::End();
}

StaticMeshEditorWindows::StaticMeshEditorWindows(std::string inWorld, Asset* inAsset)
	: AssetEditorWindow(inWorld, inAsset)
{
	hiddenProperties.push_back("SectionCount");
	skippedFields.push_back("_Vertices");
	skippedFields.push_back("_Indices");

	staticMeshEditorScene = new AdvancedScene();
	meshComp = new StaticMeshComponent(staticMeshEditorScene, (StaticMesh*)inAsset, {  });
	meshComp->SetAngle(90.f);
	meshComp->RebuildTransformData();
	DirectionalLight* dirLight = new DirectionalLight(staticMeshEditorScene);
	dirLight->lightParams.ambiant = glm::vec4(1);
	staticMeshEditorScene->UpdateFramebufferSize(256, 256);
	staticMeshEditorScene->GetCamera()->Pitch = -45;
}

void StaticMeshEditorWindows::DrawHeader()
{
	staticMeshEditorScene->GetCamera()->Yaw = (float)glfwGetTime() * 5;
	staticMeshEditorScene->GetCamera()->updateCameraVectors();
	staticMeshEditorScene->GetCamera()->SetCameraLocation(meshComp->GetWorldBounds().GetOrigin() + staticMeshEditorScene->GetCamera()->Front * meshComp->GetWorldBounds().GetBoundRadius() * -2.5f);
	staticMeshEditorScene->Draw();
	ImGui::Image((ImTextureID*)staticMeshEditorScene->GetColorBuffer(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
}

MaterialEditorWindow::MaterialEditorWindow(std::string inWorld, Asset* inAsset)
	: AssetEditorWindow(inWorld, inAsset)
{
	materialEditorScene = new AdvancedScene();
	StaticMesh* sphereMesh = AssetRegistry::FindAssetByName<StaticMesh>("SphereMesh");
	StaticMeshComponent* comp = new StaticMeshComponent(materialEditorScene, sphereMesh, { (Material*)inAsset });
	comp->SetAngle(90.f);
	comp->RebuildTransformData();
	DirectionalLight* dirLight = new DirectionalLight(materialEditorScene);
	dirLight->lightParams.ambiant = glm::vec4(1);
	materialEditorScene->UpdateFramebufferSize(256, 256);
	materialEditorScene->GetCamera()->Pitch = -45;

	hiddenProperties.push_back("TextureCount");
}

void MaterialEditorWindow::DrawHeader()
{
	materialEditorScene->GetCamera()->Yaw = (float)glfwGetTime() * 5;
	materialEditorScene->GetCamera()->updateCameraVectors();
	materialEditorScene->GetCamera()->SetCameraLocation(materialEditorScene->GetCamera()->Front * -3);
	materialEditorScene->Draw();
	ImGui::Text(std::string("Texture count : " + std::to_string(((Material*)GetAsset())->GetTextureCount())).data());
	ImGui::Image((ImTextureID*)materialEditorScene->GetColorBuffer(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
	if (ImGui::Button("Add texture"))
	{
		((Material*)GetAsset())->AddTexture();
	}
}

TextureEditorWindow::TextureEditorWindow(std::string inWorld, Asset* inAsset)
	: AssetEditorWindow(inWorld, inAsset)
{
	hiddenProperties.push_back("TextureSizeX");
	hiddenProperties.push_back("TextureSizeY");
	hiddenProperties.push_back("TextureChannelsCount");
	hiddenProperties.push_back("TextureData");
}
