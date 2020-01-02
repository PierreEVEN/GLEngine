
#include "world.h"

#include <glfw3/glfw3.h>
#include "scene.h"
#include "../Engine/inputManager.h"
#include "../Engine/debugerTool.h"
#include "../EngineLog/engineLog.h"


#include "../Camera/camera.h"
#include "../Mesh/staticMesh.h"
#include "../Mesh/staticMeshComponent.h"
#include "../Asset/AssetRegistry.h"
#include "../Lighting/pointLight.h"
#include "../Lighting/spotLight.h"
#include "../Lighting/directionalLight.h"
#include "../Shader/material.h"
#include "../ImGUI/imgui_impl_glfw.h"
#include "../ImGUI/imgui_impl_opengl3.h"

std::vector<World*> GWorlds;

World::World(GLFWwindow* inAssignedWindow)
	: assignedWindow(inAssignedWindow), worldDeltaSecond(0.0)
{
	GWorlds.push_back(this);

	CreateInputManager();
	CreateScene();

	glfwSetCursorPosCallback(assignedWindow, mouse_callback);
	glfwSetScrollCallback(assignedWindow, scroll_callback);
}

World::~World()
{
}

void World::CreateScene()
{
	mainScene = new EditorScene(this);
}

void World::CreateInputManager()
{
	inputManager = new InputManager(GetWindow());
}

double LastLightUseTime = 0.0;
void World::processInput() {
	float cameraSpeed = 2.5f * (float)worldDeltaSecond;
	if (glfwGetKey(assignedWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		GetScene()->GetCamera()->SwitchCaptureMouse();
		GetInputManager()->SetCaptureMouse(GetScene()->GetCamera()->bDoesCaptureMouse);
	}

	if (glfwGetKey(assignedWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (StaticMesh* CubeMesh = AssetRegistry::FindAssetByName<StaticMesh>("CubeMesh"))
		{
			StaticMeshComponent* newObj = new StaticMeshComponent(mainScene, CubeMesh, {}, new btBoxShape(btVector3(1, 1, 1)), false);
			newObj->SetLocation(mainScene->GetCamera()->GetCameraLocation() + mainScene->GetCamera()->GetCameraForwardVector() * glm::vec3(10.f));
		}
	}

	if (glfwGetKey(assignedWindow, GLFW_KEY_R) == GLFW_PRESS)
	{
		if (StaticMesh* CubeMesh = AssetRegistry::FindAssetByName<StaticMesh>("Bombe"))
		{
			StaticMeshComponent* newObj = new StaticMeshComponent(mainScene, CubeMesh);
			newObj->SetScale3D(SVector3(1.1f));
			newObj->SetLocation(mainScene->GetCamera()->GetCameraLocation() + mainScene->GetCamera()->GetCameraForwardVector() * glm::vec3(10.f));
		}
	}
	if (glfwGetKey(assignedWindow, GLFW_KEY_T) == GLFW_PRESS)
	{
		if (StaticMesh* CubeMesh = AssetRegistry::FindAssetByName<StaticMesh>("dragon"))
		{
			StaticMeshComponent* newObj = new StaticMeshComponent(mainScene, CubeMesh);
			newObj->SetScale3D(SVector3(1.1f));
			newObj->SetLocation(mainScene->GetCamera()->GetCameraLocation() + mainScene->GetCamera()->GetCameraForwardVector() * glm::vec3(10.f));
		}
	}
	if (glfwGetKey(assignedWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (StaticMesh* CubeMesh = AssetRegistry::FindAssetByName<StaticMesh>("CubeMesh"))
		{
			StaticMeshComponent* newObj = new StaticMeshComponent(mainScene, CubeMesh, {}, new btBoxShape(btVector3(1, 1, 1)), true);
			newObj->SetLocation(mainScene->GetCamera()->GetCameraLocation() + mainScene->GetCamera()->GetCameraForwardVector() * glm::vec3(20.f));
		}
	}

	if (glfwGetKey(assignedWindow, GLFW_KEY_W) == GLFW_PRESS)
		mainScene->GetCamera()->ProcessKeyboard(FORWARD, (float)worldDeltaSecond);
	if (glfwGetKey(assignedWindow, GLFW_KEY_S) == GLFW_PRESS)
		mainScene->GetCamera()->ProcessKeyboard(BACKWARD, (float)worldDeltaSecond);
	if (glfwGetKey(assignedWindow, GLFW_KEY_A) == GLFW_PRESS)
		mainScene->GetCamera()->ProcessKeyboard(LEFT, (float)worldDeltaSecond);
	if (glfwGetKey(assignedWindow, GLFW_KEY_D) == GLFW_PRESS)
		mainScene->GetCamera()->ProcessKeyboard(RIGHT, (float)worldDeltaSecond);
	if (glfwGetKey(assignedWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
		mainScene->GetCamera()->ProcessKeyboard(UP, (float)worldDeltaSecond);
	if (glfwGetKey(assignedWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		mainScene->GetCamera()->ProcessKeyboard(DOWN, (float)worldDeltaSecond);

	if (glfwGetKey(assignedWindow, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (glfwGetTime() - LastLightUseTime > 0.5f)
		{
			PointLight* newObj = new PointLight(mainScene);
			newObj->SetLocation(mainScene->GetCamera()->GetCameraLocation() + mainScene->GetCamera()->GetCameraForwardVector() * glm::vec3(20.f));
			newObj->lightParams.lightColor = glm::vec4(1, .8f, .6f, 1.f);
			newObj->lightParams.ambiant = newObj->lightParams.lightColor / 4.f;
			LastLightUseTime = glfwGetTime();
		}
	}

	if (glfwGetKey(assignedWindow, GLFW_KEY_K) == GLFW_PRESS)
	{
		if (glfwGetTime() - LastLightUseTime > 0.5f)
		{
			DirectionalLight* newObj = new DirectionalLight(mainScene);
			newObj->directionalLightParams.direction = glm::vec4(mainScene->GetCamera()->GetCameraForwardVector().ToGLVector(), 0.f);
			LastLightUseTime = glfwGetTime();
		}
	}

	if (glfwGetKey(assignedWindow, GLFW_KEY_J) == GLFW_PRESS)
	{
		if (glfwGetTime() - LastLightUseTime > 0.5f)
		{
			SpotLight* newObj = new SpotLight(mainScene);
			newObj->spotLightParams.direction = glm::vec4(mainScene->GetCamera()->GetCameraForwardVector().ToGLVector(), 0.f);
			newObj->SetLocation(mainScene->GetCamera()->GetCameraLocation() + mainScene->GetCamera()->GetCameraForwardVector() * glm::vec3(20.f));
			LastLightUseTime = glfwGetTime();
			if (StaticMeshComponent* lightMesh = new StaticMeshComponent(mainScene, AssetRegistry::FindAssetByName<StaticMesh>("CubeMesh"), { MaterialEditorDebuger::GetGridMaterial() }))
			{
				lightMesh->SetLocation(newObj->GetLocation());
			}
		}
	}
}

void World::UpdateScrollState(double xoffset, double yoffset)
{
	mainScene->GetCamera()->ProcessMouseScroll((float)yoffset);
}

void World::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	FindWorld(window)->mainScene->UpdateFramebufferSize(width, height);
}

void World::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	FindWorld(window)->GetInputManager()->SetMousePosition(xpos, ypos);
}

void World::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	FindWorld(window)->UpdateScrollState(xoffset, yoffset);
}


World* World::FindWorld(GLFWwindow* InWindows)
{
	if (!InWindows) return nullptr;
	for (unsigned int worldIndex = 0; worldIndex < GWorlds.size(); ++worldIndex)
	{
		if (GWorlds[worldIndex]->GetWindow() == InWindows)
		{
			return GWorlds[worldIndex];
		}
	}
	return nullptr;
}

void World::TickWorld(double newDeltaSecond)
{
	worldDeltaSecond = newDeltaSecond;

	{
		ProfileStat("Stat Inputs");
		processInput();
		GetInputManager()->ProcessInputs();
	}

	GetScene()->Draw();

	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		float lastViewportPosX, lastViewportPosY;
		EditorWindow::DrawWindow(this, lastViewportPosX, lastViewportPosY);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void World::UpdateWorlds(double deltaSecond)
{
	ProfileStat("Total @GameThread");
	for (unsigned int worldIndex = 0; worldIndex < GWorlds.size(); ++worldIndex)
	{
		GWorlds[worldIndex]->TickWorld(deltaSecond);
	}
}

std::vector<World*> World::GetWorlds()
{
	return GWorlds;
}