
#pragma once

#include <EnginePCH.h>

#define DEFAULT_BACKGROUND_CLEAR_COLOR 0, 0, 0, 1

class SceneComponent;
class Camera;
class GLDebugDrawer;
class World;

/************************************************************************/
/* Scene debug draw types                                               */
/************************************************************************/

enum class ESceneDebugDrawFlag
{
	None = 0,
	DrawBoxBounds = 1,
	DrawSphereBounds = 2
};

enum class ESceneDebugDrawMode
{
	Lit = 0,
	Wireframe = 1,
	Points = 2
};

/************************************************************************/
/* A scene handle scene components and a camera                         */
/* Update scene state with Draw() methode, then get result picture with */
/* GetColorIndex()														*/
/************************************************************************/

class Scene
{
private:

	/** Current debug mode */
	ESceneDebugDrawFlag debugDrawFlag = ESceneDebugDrawFlag::None;
	ESceneDebugDrawMode debugDrawMode = ESceneDebugDrawMode::Lit;
	/** Viewport current resolution X */
	unsigned int sceneScreenWidth = 64;
	/** Viewport current resolution Y */
	unsigned int sceneScreenHeight = 64;
	/** Viewport camera */
	Camera* sceneCamera = nullptr;
	/** Viewport debug drawer */
	GLDebugDrawer* debugDrawer = nullptr;
	/** Bakcground clear color */
	glm::vec4 defaultBackgroundColor = glm::vec4(1);
	/** Framebuffer id */
	unsigned int framebuffer = -1;
	/** Output render texture id */
	unsigned int textureColorbuffer = -1;
	/** Output RBO */
	unsigned int sceneRenderBufferObject = -1;
	/** Create framebuffer at beginning */
	bool bIsFramebufferValid = false;
public:

	/** Constructors & destroctors */
	Scene() {}
	virtual ~Scene();

	/** Scene registered scene components */
	std::vector<SceneComponent*> components;

	/** Register scene component */
	virtual void RegisterComponent(SceneComponent* inComponent);
	/** Unregister scene component */
	virtual void UnregisterComponent(SceneComponent* inComponent);

	/** Set debug display mode */
	void SetDisplayMode(ESceneDebugDrawMode inMode);
	
	/** Initialize scene after creation */
	virtual void InitializeScene();
	/** Render scene in buffer */
	virtual void Draw();
	/** Transform viewport space position to world direction */
	SVector3 PixelToWorldDirection(float screenAbsolutePosX, float screenAbsolutePosY) const;

	/** Return screen width in pixel */
	unsigned int GetScreenWidth() const { return sceneScreenWidth; }
	/** Return screen width in pixel */
	unsigned int GetScreenHeight() const { return sceneScreenHeight; }
	/** Get handled camera */
	Camera* GetCamera() const { return sceneCamera; }
	/** Get output texture id */
	unsigned int GetColorBuffer() const { return textureColorbuffer; }
	/** Get output texture buffer resolution */
	void UpdateFramebufferSize(int width, int height);
	/** Get camera projection */
	glm::mat4 GetProjection() const;
	/** Get debug flags */
	ESceneDebugDrawFlag GetDebugDrawFlag() const { return debugDrawFlag; }
	ESceneDebugDrawMode GetDebugDrawMode() const { return debugDrawMode; }
	/** Get debug drawer */
	GLDebugDrawer* GetDebugDrawer() const { return debugDrawer; }
protected:
	/** Create camera, override to use different camera class */
	virtual Camera* CreateCamera();
	/** Create debug drawer, override to use different drawer class */
	virtual GLDebugDrawer* CreateDebugDrawer();
	/** Scene is rendered in a framebuffer, created when initialized */
	virtual void CreateFramebuffer();

};

/************************************************************************/
/* Advanced scene has a background cubemap by default and advanced mouse*/
/* controls																*/
/************************************************************************/

class CubemapComponent;
class Material;

class AdvancedScene : public Scene
{
public:
	/** Constructor and destructors */
	AdvancedScene() : Scene() {}
	virtual ~AdvancedScene();

	/** Move scene viewport inside parent window */
	void SetSceneWindowLocation(int posX, int posY);

	/** Get mouse cursor location X & Y */
	float GetCursorPositionX();
	float GetCursorPositionY();

	/** Change cubemap material and create component if it hasn't been created yet */
	void SetCubemapMaterial(Material* cubemapMaterial);

	/** Called when left mouse button is pressed on current viewport */
	virtual void ClickOnScene(int posX, int posY) {}
	/** Called when right mouse button is pressed on current viewport */
	virtual void RightClickOnScene(int posX, int posY) {}
	/** Called when middle mouse button is pressed on current viewport */
	virtual void MiddleMouseClickOnScene(int posX, int posY) {}

	/** Overrided from parent */
	virtual void InitializeScene() override;
private:

	/** Current viewport location X & Y */
	int sceneWindowLocationX = 0, sceneWindowLocationY = 0;

	/** Create background cubemap component */
	void CreateCubemap();
	/** Background cubemap */
	CubemapComponent* skybox = nullptr;
};

/************************************************************************/
/* A SceneComponentIterator is used to iterate on all scene components  */                                                                  
/* of a scene by class.													*/
/* Progression start from end to beginning								*/
/* Usage :																*/
/* for (SceneComponentIterator<class> ite(DrawScene); ite; ite++) {...} */
/************************************************************************/

template<class T> class SceneComponentIterator
{
private:
	/** Current array progression index */
	int currentIndex;
	/** Handled scene */
	Scene* drawScene;
public:

	/** Default constructor */
	SceneComponentIterator(Scene* inScene)
		: drawScene(inScene), currentIndex(inScene->components.size())
	{
		MoveToNextElement();
	}

	/** True if current element is valid */
	__forceinline explicit operator bool() const
	{
		return currentIndex >= 0 && currentIndex < (int)drawScene->components.size() && drawScene->components[currentIndex];
	}

	/** true if current element isn't valid */
	__forceinline bool operator ! () const
	{
		return !(bool)*this;
	}

	/** get current element */
	__forceinline T* operator -> () const
	{
		return (T*)drawScene->components[currentIndex];
	}

	/** Get current element pointer */
	__forceinline T* operator * () const
	{
		return (T*)drawScene->components[currentIndex];
	}

	/** Get to next element */
	__forceinline void operator++ (int)
	{
		MoveToNextElement();
	}

	/** Find next element, or close iterator if end is reached */
	__forceinline void MoveToNextElement()
	{
		while (--currentIndex >= 0)
		{
			if (currentIndex >= 0 && currentIndex < (int)drawScene->components.size())
			{
				SceneComponent* usedComp = drawScene->components[currentIndex];
				if (dynamic_cast<T*>(usedComp))
				{
					break;
				}
			}
		}
	}
};