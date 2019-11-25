#ifndef WORLD_H
#define WORLD_H


#include <vector>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera;
struct GLFWwindow;
class MeshSectionComponent;
class SceneComponent;
class StaticMesh;
class Material;
class Texture2D;

class World
{

	Texture2D* defaultTexture;
	Material* defaultMaterial;
	StaticMesh* cubeMesh;
	

	Camera* worldCamera;
	GLFWwindow* window;

	unsigned int screenWidth = 800;
	unsigned int screenHeight = 600;

	double worldDeltaSecond;

	float lastX = screenWidth / 2.0f;
	float lastY = screenHeight / 2.0f;
	bool firstMouse = true;

	void UpdateWorld(double DeltaSecond);

	std::vector<SceneComponent*> primitives;

	int testWidth;
	int testHeight;


	glm::vec3 ambiantColor;

public:

	World();
	~World();

	GLFWwindow* GetWindow() const { return window; }
	Camera* GetCamera() const { return worldCamera; }
	glm::mat4 GetProjection() const;

	void AddPrimitive(SceneComponent* newPrimitive);

	void processInput();

	void UpdateFramebufferSize(int width, int height);
	void UpdateMouseState(double xpos, double ypos);
	void UpdateScrollState(double xoffset, double yoffset);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static World* FindWorld(GLFWwindow* InWindows);

	static void UpdateWorlds(double deltaSecond);
};
#endif