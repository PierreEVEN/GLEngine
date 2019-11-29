#ifndef WORLD_H
#define WORLD_H


#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Asset/asset.h"
#include <vector>

class Camera;
struct GLFWwindow;
class MeshSectionComponent;
class SceneComponent;
class StaticMesh;
class Material;
class PointLight;
class DirectionalLight;
class SpotLight;
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

	std::vector<PointLight*> pointLightSources;
	std::vector<SpotLight*> spotLightSources;
	std::vector<DirectionalLight*> directionalLightSources;
	std::vector<SceneComponent*> primitives;

	int testWidth;
	int testHeight;

	double LastLightUseTime = 0.0;

	glm::vec3 ambiantColor;

public:

	World(std::string worldName);
	~World();

	GLFWwindow* GetWindow() const { return window; }
	Camera* GetCamera() const { return worldCamera; }
	glm::mat4 GetProjection() const;

	void RegisterPrimitive(SceneComponent* newPrimitive);
	void RegisterPointLight(PointLight* newPointLightSource);
	void RegisterSpotLight(SpotLight* newSpotLightSource);
	void RegisterDirectionalLight(DirectionalLight* newDirectionalLightSource);

	std::vector<PointLight*> GetPointLightSources() const { return pointLightSources; }
	std::vector<DirectionalLight*> GetDirectionalLightSources() const { return directionalLightSources; }
	std::vector<SpotLight*> GetSpotLightSources() const { return spotLightSources; }


	void processInput();

	void UpdateFramebufferSize(int width, int height);
	void UpdateMouseState(double xpos, double ypos);
	void UpdateScrollState(double xoffset, double yoffset);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static World* FindWorld(GLFWwindow* InWindows);

	static void UpdateWorlds(double deltaSecond);

	template<class T>
	std::vector<T*> FindAssetByClass()
	{
		std::vector<T*> Elems = {};
		for (auto& primitive : primitives)
		{
			if (T* foundElement = dynamic_cast<T*>(primitive))
			{
				Elems.push_back(foundElement);
			}
		}
		return Elems;
	}

};
#endif