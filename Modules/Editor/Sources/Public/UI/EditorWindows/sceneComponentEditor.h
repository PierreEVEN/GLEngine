#pragma once

#include <UI/FloatingWindow.h>

class PrimitiveComponent;
class SceneComponent;
class World;

class SceneComponentEditor : public ImFloatingWindow
{
protected:
	World* parentWorld;
	SceneComponent* component = nullptr;
public:
	SceneComponentEditor(std::string inWindowName, World* inParentWorld);
	virtual void ClickInScene();
	virtual void Draw(World* inWorld) override;
	void DeleteSelectedComponent();
	SceneComponent* GetComponent() const { return component; }
	void SetAttachComponent(SceneComponent* inComponent);
};