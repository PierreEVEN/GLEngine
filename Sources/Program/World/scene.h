#pragma once
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "../MathLibrary/vector3.h"
#include <bullet3D/btBulletDynamicsCommon.h>
#include <iostream>

#define DEFAULT_BACKGROUND_CLEAR_COLOR 0, 0, 0, 1

class SceneComponent;
class Camera;
class GLDebugDrawer;
class Gizmo;
class World;

enum SceneDebugDrawFlag
{
	None = 0,
	DrawBoxBounds = 1,
	DrawSphereBounds = 2
};

class Scene
{
public:
	Scene();

	std::vector<SceneComponent*> components;

	virtual void RegisterComponent(SceneComponent* inComponent);
	virtual void UnregisterComponent(SceneComponent* inComponent);

protected:

	unsigned int sceneScreenWidth = 64;
	unsigned int sceneScreenHeight = 64;
	Camera* sceneCamera;
	virtual void CreateCamera();

	glm::vec4 defaultBackgroundColor;
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int sceneRenderBufferObject;
	bool bIsFramebufferValid = false;
	virtual void CreateFramebuffer();

	GLDebugDrawer* debugDrawer;
	virtual void CreateDebugDrawer();

public:

	unsigned int GetScreenWidth() const { return sceneScreenWidth; }
	unsigned int GetScreenHeight() const { return sceneScreenHeight; }
	Camera* GetCamera() const { return sceneCamera; }

	unsigned int GetColorBuffer() const { return textureColorbuffer; }
	void UpdateFramebufferSize(int width, int height);
	glm::mat4 GetProjection() const;

	GLDebugDrawer* GetDebugDrawer() const { return debugDrawer; }

	SVector3 PixelToWorldDirection(float screenAbsolutePosX, float screenAbsolutePosY) const;


	virtual void Draw();


};

class CubemapSectionComponent;
class Material;

class AdvancedScene : public Scene
{
public:
	AdvancedScene();
	
	void SetSceneWindowLocation(int posX, int posY);

	float GetCursorPositionX();
	float GetCursorPositionY();

	void SetCubemapMaterial(Material* cubemapMaterial);

	virtual void ClickOnScene(int posX, int posY) {}
	virtual void RightClickOnScene(int posX, int posY) {}
	virtual void MiddleMouseClickOnScene(int posX, int posY) {}

private:

	int sceneWindowLocationX;
	int sceneWindowLocationY;
	
	void CreateCubemap();
	CubemapSectionComponent* skybox;
};
class PhysicScene : public AdvancedScene
{
private:

	bool bSimulatePhysics;

	btDiscreteDynamicsWorld* physicsWorld;
	// La classe btBroadphaseInterface fournit une interface pour détecter les objets où leurs AABB se chevauchent.
	btBroadphaseInterface* scenePhysicsBroadphase;
	// btCollisionDispatcher supporte des algorithmes qui peuvent gérer des pairs de collisions ConvexConvex et ConvexConcave. Temps de l'impact, le point le plus proche et pénétration de profondeur.
	btCollisionDispatcher* scenePhysicsDispatcher;
	// btCollisionConfiguration permet de configurer les allocataires de mémoire.
	btDefaultCollisionConfiguration* sceneCollisionConfiguration;
	// btSequentialImpulseConstraintSolver est une implémentation SIMD rapide de la méthode Projected Gauss Seidel (iterative LCP).
	btSequentialImpulseConstraintSolver* sceneSequentialImpulseConstraintSolver;


public:
	PhysicScene();
};



class EditorScene : public PhysicScene
{
public:
	EditorScene(World* inParentWorld);

	World* GetParentWorld() const { return parentWorld; }
	Gizmo* GetGizmo() const { return sceneGizmo; }

	virtual void CreateCamera() override;
	virtual void ClickOnScene(int posX, int posY) override;

private:

	World* parentWorld = nullptr;
	Gizmo* sceneGizmo;

	void CreateGizmo();
};



template<class T> class SceneComponentIterator
{
	int size;
	int currentIndex;
	Scene* drawScene;
public:

	SceneComponentIterator(Scene* inScene)
		: drawScene(inScene), currentIndex(-1), size(inScene->components.size())
	{
		MoveToNextElement();
	}

	//Return true if element is valid
	__forceinline explicit operator bool() const
	{
		return currentIndex >= 0 && currentIndex < size && drawScene->components[currentIndex];
	}

	__forceinline bool operator ! () const
	{
		return !(bool)*this;
	}

	__forceinline T* operator -> () const
	{
		return (T*)drawScene->components[currentIndex];
	}

	__forceinline T* operator * () const
	{
		return (T*)drawScene->components[currentIndex];
	}
	__forceinline void operator++ (int)
	{
		MoveToNextElement();
	}

	__forceinline void MoveToNextElement()
	{
		while (++currentIndex < size)
		{
			if (dynamic_cast<T*>(drawScene->components[currentIndex]))
			{
				break;
			}
		}
	}

};