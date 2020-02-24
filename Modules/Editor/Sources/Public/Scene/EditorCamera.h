#pragma once
#include "Camera/camera.h"

class EditorScene;

class EditorCamera : public Camera
{
public:
	EditorCamera(EditorScene* inRenderScene, SVector3 position = SVector3(0.f), SRotator newRotation = SRotator(0.f));

	void UpdateCameraDelta();
};