#pragma once


#include <EnginePCH.h>
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
private:

	/** Wait for full destructions */
	bool bIsBeingDestroyedRenderThread = false;
	bool bIsBeingDestroyedGameThread = false;

	/** Does we need to rebuild transform data next frame */
	bool bHasTransformBeenModified = true;

	/** component transform */
	STransform componentWorldTransform;

	/** Owning scene */
	Scene* renderScene;

protected:

public:

	/** Scene component have to be registered within parent scenery */
	SceneComponent(Scene* inScene, const STransform& newComponentTransform = STransform());
	virtual ~SceneComponent();


	DrawPriority drawPriority = DrawPriority::DrawPriority_Normal;

	bool IsBeingDestroyed() const;
	bool CanBeDestroyed() const;
	void DestroyOnRenderThread();
	void DestroyOnGameThread();
	void DestroyComponent();

	virtual void DestroyRenderThread() {}
	virtual void BeginDestroy() {}
	virtual void FinnishDestroy() {}

	bool CanBeDeleted() const { return bIsBeingDestroyedRenderThread && bIsBeingDestroyedGameThread; }
	bool HasDestructionProcessBeenStarted() const { return bIsBeingDestroyedRenderThread || bIsBeingDestroyedGameThread; }
public:

	void MarkTransformDirty() { bHasTransformBeenModified = true; };

	virtual void RebuildTransformData() { bHasTransformBeenModified = false; }

	virtual void Tick(const double& inDeltaTime) {}
	virtual void Render();

	virtual void SetLocation(SVector3 newLocation);
	virtual void SetRotation(SRotator newRotation);
	virtual void SetScale3D(SVector3 newScale3D);
	virtual SVector3 GetLocation() const;
	virtual SRotator GetRotation() const;
	virtual SVector3 GetScale3D() const;

	Scene* GetScene() const;

	virtual void DrawEditorWindow();

};