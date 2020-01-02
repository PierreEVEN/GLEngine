#pragma once

#include <vector>
#include <string>

struct GLFWwindow;
class Scene;
class InputManager;

class World
{	

	GLFWwindow* assignedWindow = nullptr;
	Scene* mainScene = nullptr;
	InputManager* inputManager = nullptr;
	double worldDeltaSecond = 0.0;

	virtual void CreateScene();
	virtual void CreateInputManager();

public:

	World(GLFWwindow* inAssignedWindow);
	~World();

	Scene* GetScene() const { return mainScene; }
	GLFWwindow* GetWindow() const { return assignedWindow; }
	InputManager* GetInputManager() const { return inputManager; }
	double GetWorldDeltaSecond() const { return worldDeltaSecond; }

	void TickWorld(double newDeltaSecond);

public:

	void processInput();
	void UpdateScrollState(double xoffset, double yoffset);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static World* FindWorld(GLFWwindow* InWindows);
	static void UpdateWorlds(double deltaSecond);
	static std::vector<World*> GetWorlds();
};