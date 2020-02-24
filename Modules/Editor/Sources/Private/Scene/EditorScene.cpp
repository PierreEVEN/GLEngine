#include <Scene/EditorScene.h>
#include <Engine/inputManager.h>
#include <glfw3/glfw3.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <Physic/physicDebugViewer.h>
#include <Camera/camera.h>
#include <Scene/EditorCamera.h>
#include <Editor/EditorTools/EditorGizmo.h>
#include <World/EditorWorld.h>

EditorScene::EditorScene(EditorWorld* inParentWorld)
	: AdvancedScene()
{
	parentWorld = inParentWorld;

	inParentWorld->GetInputManager()->AddInput(new KeyBind(inParentWorld->GetInputManager(), { Key(GLFW_KEY_F1) }, ActionType::ActionType_JustPress, this, &EditorScene::SetDisplaymode_Full));
	inParentWorld->GetInputManager()->AddInput(new KeyBind(inParentWorld->GetInputManager(), { Key(GLFW_KEY_F2) }, ActionType::ActionType_JustPress, this, &EditorScene::SetDisplaymode_Wireframe));
	inParentWorld->GetInputManager()->AddInput(new KeyBind(inParentWorld->GetInputManager(), { Key(GLFW_KEY_F3) }, ActionType::ActionType_JustPress, this, &EditorScene::SetDisplaymode_WireframeTwoSided));

}

Camera* EditorScene::CreateCamera()
{
	return new EditorCamera(this, glm::vec3(0.f, 0.f, 0.f));
}

void EditorScene::SetDisplaymode_Full()
{
	SetDisplayMode(ESceneDebugDrawMode::Lit);
}

void EditorScene::SetDisplaymode_Wireframe()
{
	SetDisplayMode(ESceneDebugDrawMode::Wireframe);
}

void EditorScene::SetDisplaymode_WireframeTwoSided()
{
	SetDisplayMode(ESceneDebugDrawMode::Points);
}

void EditorScene::CreateGizmo()
{
	sceneGizmo = new EditorGizmo(this);
}