#pragma once

#include "../EditorWindow.h"
#include "../../ImGUI/imgui.h"
#include <functional>

class PrimitiveComponent;
class SceneComponent;

class SceneComponentEditor : public UIWindowElement
{
protected:
	SceneComponent* component = nullptr;
public:
	SceneComponentEditor(std::string inWindowName) : UIWindowElement(inWindowName) {}
	virtual void Draw(World* inWorld) override;
	void DeleteSelectedComponent(World* inWorld);
	SceneComponent* GetComponent() const { return component; }
	void SetAttachComponent(World* inWorld, SceneComponent* inComponent);
};