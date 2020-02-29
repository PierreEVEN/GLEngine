#include "World/EditorWorld.h"
#include <Scene/EditorScene.h>
#include <UI/EditorWindows/EditorWindow.h>
#include <Camera/camera.h>
#include <Engine/inputManager.h>
#include <Mesh/StaticMesh.h>
#include <Mesh/staticMeshComponent.h>
#include <Asset/AssetRegistry.h>
#include <Lighting/pointLight.h>
#include <Lighting/directionalLight.h>
#include <Lighting/spotLight.h>
#include <Shader/material.h>

void EditorWorld::Tick(double newDeltaSecond)
{
	processInput();
	World::Tick(newDeltaSecond);
}

void EditorWorld::Initialize()
{
	World::Initialize();

	GetInputManager()->AddInput(new KeyBind(GetInputManager(), { GLFW_KEY_Q }, ActionType_JustPress, this, &EditorWorld::PopCube));
	GetInputManager()->AddInput(new KeyBind(GetInputManager(), { GLFW_KEY_E }, ActionType_Press, this, &EditorWorld::PopCube));
}

void EditorWorld::PopCube()
{

	if (AStaticMesh* CubeMesh = AssetRegistry::FindAssetByName<AStaticMesh>("CubeMesh"))
	{
		StaticMeshComponent* newObj = new StaticMeshComponent(GetScene(), CubeMesh, {});
		newObj->SetLocation(GetScene()->GetCamera()->GetLocation() + GetScene()->GetCamera()->GetRotation().GetForwardVector() * glm::vec3(10.f));
	}
}

Scene* EditorWorld::CreateScene()
{
	Scene* createdScene = new EditorScene(this);
	return createdScene;
}

void EditorWorld::DrawUI()
{
	float lastViewportPosX, lastViewportPosY;
	EditorWindow::DrawWindow(this, lastViewportPosX, lastViewportPosY);
}


double LastLightUseTime = 0.0;
void EditorWorld::processInput() {
	float cameraSpeed = 2.5f * (float)GetWorldDeltaSecond();
	if (glfwGetKey(GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		GetScene()->GetCamera()->SwitchCaptureMouse();
		GetInputManager()->SetCaptureMouse(GetScene()->GetCamera()->bDoesCaptureMouse);
	}

	if (glfwGetKey(GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		GetScene()->GetCamera()->ProcessKeyboard(FORWARD, (float)GetWorldDeltaSecond());
	if (glfwGetKey(GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		GetScene()->GetCamera()->ProcessKeyboard(BACKWARD, (float)GetWorldDeltaSecond());
	if (glfwGetKey(GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		GetScene()->GetCamera()->ProcessKeyboard(LEFT, (float)GetWorldDeltaSecond());
	if (glfwGetKey(GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		GetScene()->GetCamera()->ProcessKeyboard(RIGHT, (float)GetWorldDeltaSecond());
	if (glfwGetKey(GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
		GetScene()->GetCamera()->ProcessKeyboard(UP, (float)GetWorldDeltaSecond());
	if (glfwGetKey(GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		GetScene()->GetCamera()->ProcessKeyboard(DOWN, (float)GetWorldDeltaSecond());

	if (glfwGetKey(GetWindow(), GLFW_KEY_L) == GLFW_PRESS)
	{
		if (glfwGetTime() - LastLightUseTime > 0.5f)
		{
			PointLight* newObj = new PointLight(GetScene());
			newObj->SetLocation(GetScene()->GetCamera()->GetLocation() + GetScene()->GetCamera()->GetRotation().GetForwardVector() * glm::vec3(20.f));
			newObj->lightParams.lightColor = glm::vec4(1, .8f, .6f, 1.f);
			newObj->lightParams.ambiant = newObj->lightParams.lightColor / 4.f;
			LastLightUseTime = glfwGetTime();
		}
	}

	if (glfwGetKey(GetWindow(), GLFW_KEY_K) == GLFW_PRESS)
	{
		if (glfwGetTime() - LastLightUseTime > 0.5f)
		{
			DirectionalLight* newObj = new DirectionalLight(GetScene());
			newObj->directionalLightParams.direction = glm::vec4(GetScene()->GetCamera()->GetRotation().GetForwardVector().ToGLVector(), 0.f);
			LastLightUseTime = glfwGetTime();
		}
	}

	if (glfwGetKey(GetWindow(), GLFW_KEY_J) == GLFW_PRESS)
	{
		if (glfwGetTime() - LastLightUseTime > 0.5f)
		{
			SpotLight* newObj = new SpotLight(GetScene());
			newObj->spotLightParams.direction = glm::vec4(GetScene()->GetCamera()->GetRotation().GetForwardVector().ToGLVector(), 0.f);
			newObj->SetLocation(GetScene()->GetCamera()->GetLocation() + GetScene()->GetCamera()->GetRotation().GetForwardVector() * glm::vec3(20.f));
			LastLightUseTime = glfwGetTime();
			if (StaticMeshComponent* lightMesh = new StaticMeshComponent(GetScene(), AssetRegistry::FindAssetByName<AStaticMesh>("CubeMesh"), { MaterialEditorDebuger::GetGridMaterial() }))
			{
				lightMesh->SetLocation(newObj->GetLocation());
			}
		}
	}
}
