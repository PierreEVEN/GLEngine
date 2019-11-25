
#include "world.h"
#include "../Camera/camera.h"

#include <glfw3/glfw3.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "../Shader/material.h"
#include "../Mesh/PhysicPrimitiveComponent.h"
#include "../Mesh/staticMesh.h"
#include "../Mesh/staticMeshComponent.h"

std::vector<World*> GWorlds;

World::World()
{
	GWorlds.push_back(this);
	worldDeltaSecond = 0.0;
	window = glfwCreateWindow(screenWidth, screenHeight, "World", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	worldCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	defaultTexture = nullptr;
	defaultMaterial = nullptr;
	cubeMesh = nullptr;
	ambiantColor = glm::vec3(0, 0, 0);
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
	projection = glm::perspective(glm::radians(worldCamera->Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 500.f);
	return projection;
}

void World::AddPrimitive(SceneComponent* newPrimitive)
{
	primitives.push_back(newPrimitive);
}

void World::processInput() {
	float cameraSpeed = 2.5f * (float)worldDeltaSecond;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{

		if (!defaultTexture) defaultTexture = new Texture2D("GLEngine/Sources/Ressources/container.jpg");
		if (!defaultMaterial) defaultMaterial = new Material("GLEngine/Sources/Shaders/defaultVertexShader.vs", "GLEngine/Sources/Shaders/defaultFragmentShaderss.fs", { defaultTexture });
		if (!cubeMesh) cubeMesh = new StaticMesh("GLEngine/Sources/Ressources/cube.obj", { defaultMaterial });

		StaticMeshComponent* newObj = new StaticMeshComponent(this, cubeMesh);
		newObj->SetLocation(worldCamera->GetCameraLocation() + worldCamera->GetCameraForwardVector() * glm::vec3(10.f));
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{

		if (!defaultTexture) defaultTexture = new Texture2D("GLEngine/Sources/Ressources/container.jpg");
		if (!defaultMaterial) defaultMaterial = new Material("GLEngine/Sources/Shaders/defaultVertexShader.vs", "GLEngine/Sources/Shaders/defaultFragmentShaderss.fs", { defaultTexture });
		if (!cubeMesh) cubeMesh = new StaticMesh("GLEngine/Sources/Ressources/cube.obj", { defaultMaterial });

		PhysicPrimitiveComponent* newObj = new PhysicPrimitiveComponent(this, &cubeMesh->meshSections[0]);
		newObj->SetLocation(worldCamera->GetCameraLocation() + worldCamera->GetCameraForwardVector() * glm::vec3(20.f));
		newObj->SetLinearVelocity(worldCamera->GetCameraForwardVector() * glm::vec3(10.f));
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
	// create transformations
	glm::mat4 projection = glm::mat4(1.0f);
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(worldCamera->Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 500.f);

	glClearColor(50 / 256.0, 50 / 256.0, 50 / 256.0, 1.0f);
	glClearColor(115 / 256.0, 243 / 256.0, 243 / 256.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	processInput();

	for (unsigned int primIndex = 0; primIndex < primitives.size(); ++primIndex)
	{
		primitives[primIndex]->MarkRenderStateDirty();
		primitives[primIndex]->Update(worldDeltaSecond);
	}

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