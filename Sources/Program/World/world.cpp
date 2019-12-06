
#include "world.h"
#include "../Camera/camera.h"

#include <glfw3/glfw3.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "../Shader/material.h"
#include "../Mesh/PhysicPrimitiveComponent.h"
#include "../Mesh/staticMesh.h"
#include "../Mesh/staticMeshComponent.h"
#include "../Asset/assetLibrary.h"
#include "../Lighting/pointLight.h"
#include "../Lighting/directionalLight.h"
#include "../Lighting/spotLight.h"

#include <bullet3D/btBulletDynamicsCommon.h>
#include <bullet3D/LinearMath/btIDebugDraw.h>
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_opengl3.h"
#include "../ImGUI/imgui_impl_glfw.h"
#include "../UI/EditorWindow.h"
#include "../Asset/AssetRegistry.h"

std::vector<World*> GWorlds;

World::World(std::string worldName)
{
	GWorlds.push_back(this);
	worldDeltaSecond = 0.0;
	std::string viewportName = std::string("GLEngine 0.1 - ") + worldName;
	window = glfwCreateWindow(screenWidth, screenHeight, viewportName.data(), NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	worldCamera = new Camera(this, glm::vec3(0.0f, 0.0f, 3.0f));

	/************************************************************************/
	/* BULLET 3D                                                            */
	/************************************************************************/

	///collision configuration contains default setup for memory, collision setup
	myCollisionConfiguration = new btDefaultCollisionConfiguration();
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	myDispatcher = new	btCollisionDispatcher(myCollisionConfiguration);
	myBroadphase = new btDbvtBroadphase();
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mySequentialImpulseConstraintSolver = new btSequentialImpulseConstraintSolver;

	physicWorld = new btDiscreteDynamicsWorld(myDispatcher, myBroadphase, mySequentialImpulseConstraintSolver, myCollisionConfiguration);
	// On définit la gravité, de façon à ce que les objets tombent vers le bas (-Y).
	physicWorld->setGravity(btVector3(0, 0, -10));


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	defaultTexture = nullptr;
	defaultMaterial = nullptr;
	cubeMesh = nullptr;


}

World::~World()
{
	for (unsigned int i = 0; i < GWorlds.size(); ++i)
	{
		if (GWorlds[i] == this)
		{
			GWorlds.erase(GWorlds.begin() + i);
			break;
		}
	}
	for (unsigned int i = 0; i < primitives.size(); ++i)
	{
		delete primitives[i];
	}
}

glm::mat4 World::GetProjection() const
{
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(worldCamera->Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 5000.f);
	return projection;
}

void World::RegisterPrimitive(SceneComponent* newPrimitive)
{
	primitives.push_back(newPrimitive);
}

void World::RegisterPointLight(PointLight* newPointLightSource)
{
	pointLightSources.push_back(newPointLightSource);
}

void World::RegisterSpotLight(SpotLight* newSpotLightSource)
{
	spotLightSources.push_back(newSpotLightSource);
}

void World::RegisterDirectionalLight(DirectionalLight* newDirectionalLightSource)
{
	directionalLightSources.push_back(newDirectionalLightSource);
}

void World::processInput() {
	float cameraSpeed = 2.5f * (float)worldDeltaSecond;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		GetCamera()->SwitchCaptureMouse();

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		StaticMeshComponent* newObj = new StaticMeshComponent(this, AssetRegistry::FindAssetByName<StaticMesh>("CubeMesh"));
		newObj->SetLocation(worldCamera->GetCameraLocation() + worldCamera->GetCameraForwardVector() * glm::vec3(10.f));
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		PhysicPrimitiveComponent* newObj = new PhysicPrimitiveComponent(this, &AssetRegistry::FindAssetByName<StaticMesh>("CubeMesh")->meshSections[0]);
		newObj->SetLocation(worldCamera->GetCameraLocation() + worldCamera->GetCameraForwardVector() * glm::vec3(20.f));
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		worldCamera->ProcessKeyboard(FORWARD, (float)worldDeltaSecond);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		worldCamera->ProcessKeyboard(BACKWARD, (float)worldDeltaSecond);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		worldCamera->ProcessKeyboard(LEFT, (float)worldDeltaSecond);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		worldCamera->ProcessKeyboard(RIGHT, (float)worldDeltaSecond);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		worldCamera->ProcessKeyboard(UP, (float)worldDeltaSecond);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		worldCamera->ProcessKeyboard(DOWN, (float)worldDeltaSecond);
	

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (glfwGetTime() - LastLightUseTime > 0.5f)
		{
			PointLight* newObj = new PointLight(this);
			StaticMeshComponent* lightMesh = new StaticMeshComponent(this, AssetRegistry::FindAssetByName<StaticMesh>("CubeMesh"));
			newObj->SetLocation(worldCamera->GetCameraLocation() + worldCamera->GetCameraForwardVector() * glm::vec3(20.f));
			newObj->ambiant = glm::normalize(glm::vec3(rand() % 512 / 256.f, rand() % 512 / 256.f, rand() % 512 / 256.f));
			lightMesh->SetLocation(newObj->GetLocation());
			LastLightUseTime = glfwGetTime();
		}
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		if (glfwGetTime() - LastLightUseTime > 0.5f)
		{
			DirectionalLight* newObj = new DirectionalLight(this);
			newObj->direction = GetCamera()->GetCameraForwardVector().ToGLVector();
			LastLightUseTime = glfwGetTime();
		}
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		if (glfwGetTime() - LastLightUseTime > 0.5f)
		{
			SpotLight* newObj = new SpotLight(this);
			StaticMeshComponent* lightMesh = new StaticMeshComponent(this, AssetRegistry::FindAssetByName<StaticMesh>("CubeMesh"));
			newObj->direction = GetCamera()->GetCameraForwardVector().ToGLVector();
			newObj->SetLocation(worldCamera->GetCameraLocation() + worldCamera->GetCameraForwardVector() * glm::vec3(20.f));
			lightMesh->SetLocation(newObj->GetLocation());
			LastLightUseTime = glfwGetTime();
		}
	}
}

void World::UpdateFramebufferSize(int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;

}

void World::UpdateMouseState(double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

	lastX = (float)xpos;
	lastY = (float)ypos;

	worldCamera->ProcessMouseMovement(xoffset, yoffset);
}

void World::UpdateScrollState(double xoffset, double yoffset)
{
	worldCamera->ProcessMouseScroll((float)yoffset);
}

void World::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	FindWorld(window)->UpdateFramebufferSize(width, height);
}

void World::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	FindWorld(window)->UpdateMouseState(xpos, ypos);
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

void World::UpdateWorld(double deltaSecond)
{
	worldDeltaSecond = deltaSecond;



	if (GetPhysicWorld() && bSimulatePhysics)
	{
		GetPhysicWorld()->stepSimulation((float)deltaSecond);
	}


	// create transformations
	glm::mat4 projection = glm::mat4(1.0f);
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(worldCamera->Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 500.f);
	
	glClearColor(50 / 256.0, 50 / 256.0, 50 / 256.0, 1.0f);
	glClearColor(115 / 256.0, 243 / 256.0, 243 / 256.0, 1.0f);
	//glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);



	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	processInput();

	for (unsigned int primIndex = 0; primIndex < primitives.size(); ++primIndex)
	{
		primitives[primIndex]->MarkRenderStateDirty();
		primitives[primIndex]->Update(worldDeltaSecond);
	}
	
	EditorWindow::DrawWindow(this);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(GetWindow());
	glfwPollEvents();
}

void World::UpdateWorlds(double deltaSecond)
{
	for (unsigned int worldIndex = 0; worldIndex < GWorlds.size(); ++worldIndex)
	{
		GWorlds[worldIndex]->UpdateWorld(deltaSecond);
	}
}
