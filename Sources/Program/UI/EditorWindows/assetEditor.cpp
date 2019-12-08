#include "assetEditor.h"
#include "../../Asset/asset.h"
#include "../../ImGUI/imgui.h"
#include <iostream>
#include "../../Asset/GLAssetIO.h"

AssetEditorWindow::AssetEditorWindow(Asset* inAsset)
 : UIWindowElement()
{
	linkedAsset = inAsset;
}

void AssetEditorWindow::DisplayProperty(SPropertyValue* inProperty)
{
	if (!inProperty || !inProperty->IsValid())
	{
		return;
	}
	if (inProperty->GetPropertyType() == PropertyTypeBool)
	{
		DisplayBoolProperty((SBoolPropertyValue*)inProperty);
	}
	else if (inProperty->GetPropertyType() == PropertyTypeString)
	{
		DisplayStringProperty((SStringPropertyValue*)inProperty);
	}
	else if (inProperty->GetPropertyType() == PropertyTypeInt)
	{
		DisplayIntProperty((SIntPropertyValue*)inProperty);
	}
	else
	{
		DisplayEmptyProp(inProperty);
	}
}

void AssetEditorWindow::DisplayEmptyProp(SPropertyValue* inProperty)
{
	ImGui::Text(std::string(inProperty->GetPropertyName() + " : none").data());
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
		//std::cout << " new text : " << testVal << " safe : " << inProperty->GetStringValue() << std::endl;
	}

	//delete propertySize;
}

void AssetEditorWindow::DisplayIntProperty(SIntPropertyValue* inProperty)
{
	int localValue = inProperty->GetIntValue();
	ImGui::SliderInt(inProperty->GetPropertyName().data(), &localValue, 0, 1000);

	if (localValue != inProperty->GetIntValue())
	{
		inProperty->SetIntValue(localValue);
	}

}

void StaticMeshEditorWindows::Draw(World* inWorld)
{
	UIWindowElement::Draw(inWorld);
	ImGui::Begin(std::string(GetAsset()->GetName() + " editor").data(), &bKeepOpen);
	for (const auto& currentProperty : GetAsset()->GetAssetProperties())
	{
		DisplayProperty(currentProperty);
	}
	ImGui::End();

}
