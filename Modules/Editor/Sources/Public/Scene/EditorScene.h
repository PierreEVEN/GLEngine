#pragma once

#include <Scene/scene.h>


class EditorGizmo;
class EditorWorld;

class EditorScene : public AdvancedScene
{
public:
	EditorScene(EditorWorld* inParentWorld);

	EditorWorld* GetParentWorld() const { return parentWorld; }
	EditorGizmo* GetGizmo() const { return sceneGizmo; }

	virtual Camera* CreateCamera() override;

private:

	void SetDisplaymode_Full();
	void SetDisplaymode_Wireframe();
	void SetDisplaymode_WireframeTwoSided();

	EditorWorld* parentWorld = nullptr;
	EditorGizmo* sceneGizmo;

	void CreateGizmo();
};
