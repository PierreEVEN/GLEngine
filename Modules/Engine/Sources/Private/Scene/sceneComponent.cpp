
#include <imgui.h>
#include <Scene/scene.h>
#include <Scene/sceneComponent.h>

SceneComponent::SceneComponent(Scene* inScene, const STransform& newComponentTransform)
	: renderScene(inScene)
{
	assert(inScene);
	componentWorldTransform = newComponentTransform;
	inScene->RegisterComponent(this);
}

SceneComponent::~SceneComponent() 
{
	/** Scene component can only be destroyed with DestroyComponent() */
	if (!CanBeDestroyed()) GLogAssert("Warning, cannot delete scene component before full destruction");
	GetScene()->UnregisterComponent(this);
}

bool SceneComponent::IsBeingDestroyed() const
{
	return bIsBeingDestroyedGameThread || bIsBeingDestroyedRenderThread;
}

bool SceneComponent::CanBeDestroyed() const
{
	return bIsBeingDestroyedGameThread && bIsBeingDestroyedRenderThread;
}

void SceneComponent::DestroyOnRenderThread()
{
	CHECK_RENDER_THREAD;
	if (!bIsBeingDestroyedRenderThread)
	{
		bIsBeingDestroyedRenderThread = true;
		DestroyRenderThread();
	}
}

void SceneComponent::DestroyOnGameThread()
{
	CHECK_GAME_THREAD;
	if (!bIsBeingDestroyedGameThread)
	{
		bIsBeingDestroyedGameThread = true;
		BeginDestroy();
	}
}

void SceneComponent::DestroyComponent()
{
	CHECK_GAME_THREAD;
	DestroyOnGameThread();
}

void SceneComponent::Render()
{
	if (bHasTransformBeenModified) RebuildTransformData();
}

void SceneComponent::SetLocation(SVector3 newLocation) 
{
	componentWorldTransform.SetLocation(newLocation);
	MarkTransformDirty();
}
void SceneComponent::SetRotation(SRotator newRotation)
{
	componentWorldTransform.SetRotation(newRotation);
	MarkTransformDirty();
}
void SceneComponent::SetScale3D(SVector3 newScale3D)
{
	componentWorldTransform.SetScale3D(newScale3D);
	MarkTransformDirty();
}

SVector3 SceneComponent::GetLocation() const { return componentWorldTransform.GetLocation(); }
SRotator SceneComponent::GetRotation() const { return componentWorldTransform.GetRotation(); }
SVector3 SceneComponent::GetScale3D() const { return componentWorldTransform.GetScale3D(); }

Scene* SceneComponent::GetScene() const { return renderScene; }

void SceneComponent::DrawEditorWindow()
{
	ImGui::Separator();
	ImGui::Text("component");

	float pos[3] = { (float)GetLocation().x, (float)GetLocation().y, (float)GetLocation().z };
	ImGui::SliderFloat3("position slider", pos, -100, 100);
	ImGui::InputFloat3("position text", pos);
	if (pos[0] != (float)GetLocation().x ||
		pos[1] != (float)GetLocation().y ||
		pos[2] != (float)GetLocation().z)
	{
		SetLocation(SVector3(pos[0], pos[1], pos[2]));
	}

	float scale[3] = { (float)GetScale3D().x, (float)GetScale3D().y, (float)GetScale3D().z };
	ImGui::SliderFloat3("scale slider", scale, 0, 10);
	ImGui::InputFloat3("scale text", scale);
	if (scale[0] != (float)GetScale3D().x ||
		scale[1] != (float)GetScale3D().y ||
		scale[2] != (float)GetScale3D().z)
	{
		SetScale3D(SVector3(scale[0], scale[1], scale[2]));
	}
	
	float rotVec[3] = { (float)fmod((GetRotation().GetRoll() + 360), 360), (float)fmod((float)GetRotation().GetPitch() + 360, 360), (float)fmod((float)GetRotation().GetYaw() + 360, 360) };
	ImGui::SliderFloat3("rot vec slider", rotVec, 0, 360);
	ImGui::InputFloat3("rot vec text", rotVec);
	if (SRotator(rotVec[0], rotVec[1], rotVec[2]) != GetRotation())
	{
		SetRotation(SRotator(rotVec[0], rotVec[1], rotVec[2]));
	}
}