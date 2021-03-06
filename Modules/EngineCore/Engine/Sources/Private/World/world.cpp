#include <World/world.h>
#include <Engine/inputManager.h>
#include <Camera/camera.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <Scene/scene.h>
#include <Engine/engine.h>
#include <Engine/ThreadHandler.h>

/** Static registered world vector */
std::vector<World*> GWorlds;

World::World(GLFWwindow* inAssignedWindow)
	: assignedWindow(inAssignedWindow) {
	GWorlds.push_back(this);
}

World::~World() {
	if (mainScene) delete mainScene;
	if (inputManager) delete inputManager;
	for (unsigned int i = 0; i < GWorlds.size(); ++i)
	{
		if (GWorlds[i] == this)
		{
			GWorlds.erase(GWorlds.begin() + i);
			return;
		}
	}

}

void World::Initialize() {
	CHECK_GAME_THREAD;
	/** World cannot be initialized twice */
	if (bHasBeenInitialized) GFullLog(LogVerbosity::Assert, "World", "This world is already initialized");
	bHasBeenInitialized = true;

	inputManager = CreateInputManager();
	mainScene = CreateScene();

	glfwSetCursorPosCallback(assignedWindow, mouse_callback);
	glfwSetScrollCallback(assignedWindow, scroll_callback);
}

Scene* World::CreateScene() {
	Scene* createdScene = new Scene();
	return createdScene;
}

InputManager* World::CreateInputManager() {
	return new InputManager(GetWindow());
}

void World::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	FindWorld(window)->mainScene->UpdateFramebufferSize(width, height);
}

void World::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	FindWorld(window)->GetInputManager()->SetMousePosition(xpos, ypos);
}

void World::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	FindWorld(window)->GetScene()->GetCamera()->ProcessMouseScroll((float)yoffset);
}

World* World::FindWorld(GLFWwindow* InWindows) {
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

void World::Tick(double newDeltaSecond) {

	CHECK_GAME_THREAD;

	if (!bHasBeenInitialized) Initialize();

	worldDeltaSecond = newDeltaSecond;

	/** Get inputs */
	if (GetInputManager()) GetInputManager()->ProcessInputs();

	if (GetScene())
	{
		GetScene()->Tick(worldDeltaSecond);
	}
}

void World::Render()
{
	if (!GetScene()) return;
	/** Redraw Scene */
	GetScene()->Render();

	/** Draw interface */
	{
		ProfileStat("Draw interface", "ImGui");
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		DrawUI();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	if (glfwWindowShouldClose(GetWindow()))
	{
		Engine::ShutDown();
	}
}

void World::TickWorlds(double deltaSecond) {
	for (unsigned int worldIndex = 0; worldIndex < GWorlds.size(); ++worldIndex) {
		GWorlds[worldIndex]->Tick(deltaSecond);
	}
}

void World::RenderWorlds()
{
	for (unsigned int worldIndex = 0; worldIndex < GWorlds.size(); ++worldIndex) {
		GWorlds[worldIndex]->Render();
	}
}

void World::ClearWorlds() {
	for (int worldIndex = (int)GetWorlds().size() - 1; worldIndex >= 0; --worldIndex) {
		delete GetWorlds()[worldIndex];
	}
}

std::vector<World*> World::GetWorlds() {
	return GWorlds;
}