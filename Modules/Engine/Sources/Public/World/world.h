
/************************************************************************/
/* Each game window has a linked world.									*/
/* A world handle an unique scene and game inputs						*/
/************************************************************************/

#pragma once

#include <EnginePCH.h>

struct GLFWwindow;
class Scene;
class InputManager;

class World
{
private:
	/** Parent window */
	GLFWwindow* assignedWindow = nullptr;
	/** Handled scene */
	Scene* mainScene = nullptr;
	/** Handled input manager */
	InputManager* inputManager = nullptr;
	/** Last world delta */
	double worldDeltaSecond = 0.0;
	/** has Initialize() been called */
	bool bHasBeenInitialized = false;

protected:

	/** Override to use your own scene */
	virtual Scene* CreateScene();
	/** Override to use your own Input manager */
	virtual InputManager* CreateInputManager();

	/** Implement to add ui elements to draw */
	virtual void DrawUI() {}

public:

	/** Constructors and destructors */
	World(GLFWwindow* inAssignedWindow);
	virtual ~World();

	/** Initializer has to be called once the world is created. */
	virtual void Initialize();
	/** Update world every frame with current delta second on Game Thread*/
	virtual void Tick(double newDeltaSecond);
	/** Redraw world */
	virtual void Render();

	/** Get last delta second */
	double GetWorldDeltaSecond() const { return worldDeltaSecond; }
	/** Get handled scene */
	Scene* GetScene() const { return mainScene; }
	/** Get parent window */
	GLFWwindow* GetWindow() const { return assignedWindow; }
	/** Get handled input manager */
	InputManager* GetInputManager() const { return inputManager; }

	/** Get a list of currently registered worlds */
	static std::vector<World*> GetWorlds();
	/** Get a world from GLFW Window */
	static World* FindWorld(GLFWwindow* InWindows);
	/** Refresh all registered worlds on game thread */
	static void TickWorlds(double deltaSecond);
	/** Refresh all registered worlds on render thread */
	static void RenderWorlds();
	/** Destroy all existing worlds */
	static void ClearWorlds();
	/** called on windows size changed */
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	/** called on mouse position changed */
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	/** called on mouse scroll used */
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};