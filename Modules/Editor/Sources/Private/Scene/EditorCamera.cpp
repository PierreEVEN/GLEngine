#include "Scene/EditorCamera.h"
#include <Scene/EditorScene.h>
#include <World/EditorWorld.h>
#include <Engine/inputManager.h>

EditorCamera::EditorCamera(EditorScene* inRenderScene, SVector3 position, SRotator newRotation)
	: Camera(inRenderScene, position, newRotation)
{
	inRenderScene->GetParentWorld()->GetInputManager()->BindOnMousePositionChanged(this, &EditorCamera::UpdateCameraDelta);
}

void EditorCamera::UpdateCameraDelta()
{
	ProcessMouseMovement(-((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMousePositionDeltaX(), ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMousePositionDeltaY());
}
