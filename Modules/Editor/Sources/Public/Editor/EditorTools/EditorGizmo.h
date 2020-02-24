#pragma once

#include <Scene/primitiveComponent.h>

class EditorScene;
struct Vertex;
struct SVector3;

enum class EGizmoTranslationType
{
	None = 0,
	X = 1,
	XY = 2,
	XZ = 3,
	Y = 4,
	YZ = 5,
	Z = 6,
	All = 7
};

enum class EGizmoMode
{
	None = 0,
	Translate = 1,
	Rotate = 2,
	Scale = 3
};

class EditorGizmo : public PrimitiveComponent
{
public:

	bool bIsDraging = false;
	unsigned int TranslationVAO, TranlsationEBO, TranslationVBO;
	unsigned int RotationVAO, RotationEBO, RotationVBO;
	unsigned int ScaleVAO, ScaleEBO, ScaleVBO;
	std::vector<Vertex> TranslationVerticesArray;
	std::vector<unsigned int> TranslationTrianglesArray;
	std::vector<Vertex> RotationVerticesArray;
	std::vector<unsigned int> RotationTriangleArray;
	std::vector<Vertex> ScaleVerticesArray;
	std::vector<unsigned int> ScaleTriangleArray;
	EditorGizmo(EditorScene* inScene);
	virtual void Tick() override;
	SceneComponent* attachedComponent = nullptr;
	void SetCanDrag(bool bInDrag);
	void BeginDrag();
	void EndDrag();
	EGizmoMode gizmoMode = EGizmoMode::Rotate;
	EGizmoTranslationType gizmoTranslationType;
	void AttachToComponent(SceneComponent* inComponent);

	void SetTranslateMode() { SwitchGizmoMode(EGizmoMode::Translate); }
	void SetRotateMode() { SwitchGizmoMode(EGizmoMode::Rotate); }
	void SetScaleMode() { SwitchGizmoMode(EGizmoMode::Scale); }
	void SwitchGizmoMode(const EGizmoMode& inNewMode);

	SVector3 LocalScale;
	SRotator LocalRotation;
	SVector3 RotationStartDragPoint;

	EGizmoTranslationType GetTranslationTypeFromCamera(SVector3 startPos, SVector3 direction);
	EGizmoTranslationType GetRotationTypeFromCamera(SVector3 startPos, SVector3 direction);

	virtual void Drag();
};