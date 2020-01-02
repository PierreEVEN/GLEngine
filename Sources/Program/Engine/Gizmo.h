#pragma once

#include "../Physic/primitiveComponent.h"

class EditorScene;

enum class DragType
{
	None = 0,
	DirX = 1,
	DirY = 2,
	DirZ = 3
};

class Gizmo : public PrimitiveComponent
{
public:

	DragType currentDragDirection = DragType::None;

	bool bIsDraging = false;
	unsigned int VAO, EBO, VBO;
	Gizmo(EditorScene* inScene);
	virtual void Tick() override;
	SceneComponent* attachedComponent = nullptr;
	void SetCanDrag(bool bInDrag);
	void BeginDrag();
	void EndDrag();
	int currentState;
	void AttachToComponent(SceneComponent* inComponent);

	virtual void Drag();
};