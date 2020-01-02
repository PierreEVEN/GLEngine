#include "sceneComponent.h"
#include "world.h"
#include "scene.h"
#include "../ImGUI/imgui.h"

SceneComponent::SceneComponent(Scene* inScene)
{
	assert(inScene);
	location = SVector3(0);
	rotation = SRotator(0);
	forwardVector = glm::vec3(1.f, 0.f, 0.f);
	angle = 0.f;
	scale3D = SVector3(1);
	renderScene = inScene;
	inScene->RegisterComponent(this);
}

SceneComponent::~SceneComponent() 
{
	GetScene()->UnregisterComponent(this);
}

void SceneComponent::SetLocation(SVector3 newLocation) 
{
	location = newLocation;
	MarkTransformDirty();
}
void SceneComponent::SetRotation(SRotator newRotation)
{
	rotation = newRotation;
	MarkTransformDirty();
}
void SceneComponent::SetForwardVector(glm::vec3 newForwardVector)
{
	forwardVector = newForwardVector;
	MarkTransformDirty();
}
void SceneComponent::SetAngle(float newAngle) 
{
	angle = newAngle;
	MarkTransformDirty();
}
void SceneComponent::SetScale3D(SVector3 newScale3D)
{
	scale3D = newScale3D;
	MarkTransformDirty();
}

SVector3 SceneComponent::GetLocation() const { return location; }
SRotator SceneComponent::GetRotation() const { return rotation; }
glm::vec3 SceneComponent::GetForwardVector() const { return forwardVector; }
float SceneComponent::GetAngle() const { return angle; }
SVector3 SceneComponent::GetScale3D() const { return scale3D; }

Scene* SceneComponent::GetScene() { return renderScene; }

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
	ImGui::SliderFloat3("scale slider", scale, -100, 100);
	ImGui::InputFloat3("scale text", scale);
	if (scale[0] != (float)GetScale3D().x ||
		scale[1] != (float)GetScale3D().y ||
		scale[2] != (float)GetScale3D().z)
	{
		SetScale3D(SVector3(scale[0], scale[1], scale[2]));
	}

	float rotVec[3] = { (float)GetForwardVector().x, (float)GetForwardVector().y, (float)GetForwardVector().z };
	ImGui::SliderFloat3("rot vec slider", rotVec, -1, 1);
	ImGui::InputFloat3("rot vec text", rotVec);
	if (rotVec[0] != (float)GetForwardVector().x ||
		rotVec[1] != (float)GetForwardVector().y ||
		rotVec[2] != (float)GetForwardVector().z)
	{
		SetForwardVector(SVector3(rotVec[0], rotVec[1], rotVec[2]).ToGLVector());
	}

	float rotAngle = GetAngle();
	ImGui::SliderFloat("rot angle slider", &rotAngle, 0, 360);
	ImGui::InputFloat("rot angle text", &rotAngle);
	if (rotAngle != GetAngle())
	{
		SetAngle(rotAngle);
	}
}
