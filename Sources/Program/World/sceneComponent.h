#pragma once

#include <glm/gtx/matrix_decompose.hpp>
#include "../MathLibrary/vector3.h"

class Scene;

enum DrawPriority
{
	DrawPriority_First = 0,
	DrawPriority_Normal = 1,
	DrawPriority_Last = 2,
	DrawPriority_Max = 3
};

class SceneComponent
{
public:

	SceneComponent(Scene* inScene);
	~SceneComponent();

	DrawPriority drawPriority = DrawPriority::DrawPriority_Normal;

protected:

	Scene* renderScene;

	SVector3 location;
	SRotator rotation;
	glm::vec3 forwardVector;
	float angle;
	SVector3 scale3D;
	bool bHasTransformBeenModified = true;
public:

	virtual void RebuildTransformData() { bHasTransformBeenModified = false; }

	void MarkTransformDirty() { bHasTransformBeenModified = true; };
	virtual void Tick() { if (bHasTransformBeenModified) RebuildTransformData(); };

	virtual void SetLocation(SVector3 newLocation);
	virtual void SetRotation(SRotator newRotation);
	virtual void SetForwardVector(glm::vec3 newForwardVector);
	virtual void SetAngle(float newAngle);
	virtual void SetScale3D(SVector3 newScale3D);

	virtual SVector3 GetLocation() const;
	virtual SRotator GetRotation() const;
	virtual glm::vec3 GetForwardVector() const;
	virtual float GetAngle() const;
	virtual SVector3 GetScale3D() const;
	Scene* GetScene();

	virtual void DrawEditorWindow();

};