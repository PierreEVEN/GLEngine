#include "scene.h"
#include "sceneComponent.h"
#include <glad/glad.h>
#include "../EngineLog/engineLog.h"
#include "../Camera/camera.h"
#include "../Physic/physicDebugViewer.h"
#include "../Engine/debugerTool.h"
#include "../Shader/material.h"
#include <glfw3/glfw3.h>

#include "../Asset/AssetRegistry.h"
#include "../Mesh/meshSectionComponent.h"
#include "../Mesh/staticMesh.h"
#include "../Engine/Gizmo.h"
#include "world.h"

unsigned int lastGLScreenSizeX = 0, lastGLScreenSizeY = 0;

Scene::Scene()
{
	defaultBackgroundColor = glm::vec4(DEFAULT_BACKGROUND_CLEAR_COLOR);
	components = {};
	framebuffer = -1;
	textureColorbuffer = -1;
	sceneRenderBufferObject = -1;
	bIsFramebufferValid = false;

	CreateFramebuffer();
	CreateDebugDrawer();
	CreateCamera();
}


void Scene::CreateFramebuffer()
{
	if (!bIsFramebufferValid)
	{
		glGenFramebuffers(1, &framebuffer);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	if (!bIsFramebufferValid) glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sceneScreenWidth, sceneScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	if (!bIsFramebufferValid) glGenRenderbuffers(1, &sceneRenderBufferObject);
	glBindRenderbuffer(GL_RENDERBUFFER, sceneRenderBufferObject);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sceneScreenWidth, sceneScreenHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneRenderBufferObject); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		GFullLog(LogVerbosity::Assert, "LogScene", "failed to create frameBuffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	bIsFramebufferValid = true;
}

void Scene::UpdateFramebufferSize(int width, int height)
{
	glViewport(0, 0, width, height);
	sceneScreenWidth = width;
	sceneScreenHeight = height;
	CreateFramebuffer();
}

void Scene::CreateDebugDrawer()
{
	debugDrawer = new GLDebugDrawer(this);
}

void Scene::CreateCamera()
{
	sceneCamera = new Camera(this, glm::vec3(0.f, 0.f, 0.f));
}

glm::mat4 Scene::GetProjection() const
{
	return glm::perspective(glm::radians(sceneCamera->Zoom), (float)sceneScreenWidth / (float)sceneScreenHeight, sceneCamera->Near, sceneCamera->Far);
}

SVector3 Scene::PixelToWorldDirection(float screenAbsolutePosX, float screenAbsolutePosY) const
{
	float localPosX = (screenAbsolutePosX / sceneScreenWidth) * 2 - 1;
	float localPosY = -1 * ((screenAbsolutePosY / sceneScreenHeight) * 2 - 1);

	glm::mat4 invMat = glm::inverse(GetProjection() * GetCamera()->GetViewMatrix());
	glm::vec4 near = glm::vec4(localPosX, localPosY, -1, 1.0);
	glm::vec4 far = glm::vec4(localPosX, localPosY, 1, 1.0);
	glm::vec4 nearResult = invMat * near;
	glm::vec4 farResult = invMat * far;
	nearResult /= nearResult.w;
	farResult /= farResult.w;
	glm::vec3 dir = glm::vec3(farResult - nearResult);
	return glm::normalize(dir);
}

void Scene::RegisterComponent(SceneComponent* inComponent)
{
	components.push_back(inComponent);
}

void Scene::UnregisterComponent(SceneComponent* inComponent)
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i] == inComponent)
		{
			components.erase(components.begin() + i);
			return;
		}
	}
}

void Scene::Draw()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	ProfileStat("Draw scene");
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	if (lastGLScreenSizeX != sceneScreenWidth || lastGLScreenSizeY != sceneScreenHeight)
	{
		lastGLScreenSizeX = sceneScreenWidth;
		lastGLScreenSizeY = sceneScreenHeight;
		glViewport(0, 0, sceneScreenWidth, sceneScreenHeight);
	}
	glClearColor(defaultBackgroundColor.x, defaultBackgroundColor.y, defaultBackgroundColor.z, defaultBackgroundColor.w);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Material::UpdateMaterialDefaults(this);
	for (int i = 0; i < DrawPriority::DrawPriority_Max; ++i)
	{
		for (SceneComponentIterator<SceneComponent> ite(this); ite; ite++)
		{
			if ((*ite)->drawPriority == i) (*ite)->Tick();
		}
	}

	GetDebugDrawer()->RenderDebugDraw();

	glDisable(GL_FRAMEBUFFER_SRGB);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

AdvancedScene::AdvancedScene()
	: Scene()
{
	CreateCubemap();
}

void AdvancedScene::SetSceneWindowLocation(int posX, int posY)
{
	sceneWindowLocationX = posX;
	sceneWindowLocationY = posY;
}

float AdvancedScene::GetCursorPositionX()
{
	return ImGui::GetMousePos().x - sceneWindowLocationX;
}

float AdvancedScene::GetCursorPositionY()
{
	return ImGui::GetMousePos().y - sceneWindowLocationY;
}

void AdvancedScene::SetCubemapMaterial(Material* cubemapMaterial)
{
	if (skybox)	skybox->materialOverride = cubemapMaterial;
}

void AdvancedScene::CreateCubemap()
{
	if (StaticMesh* cubeMesh = AssetRegistry::FindAssetByName<StaticMesh>("CubeMesh"))
	{
		cubeMesh->LoadData();
		skybox = new CubemapSectionComponent(this, nullptr, cubeMesh->GetSections()[0]);
		skybox->SetAngle(-90.f);
	}
	else
	{
		skybox = nullptr;
	}
	SetCubemapMaterial(AssetRegistry::FindAssetByName<Material>("CubemapMaterial"));
}

EditorScene::EditorScene(World* inParentWorld)
	: PhysicScene()
{
	parentWorld = inParentWorld;
	CreateGizmo();
	CreateCamera();
}

void EditorScene::CreateCamera()
{
	sceneCamera = new EditorCamera(this, glm::vec3(0.f, 0.f, 0.f));
}

void EditorScene::ClickOnScene(int posX, int posY)
{

}

void EditorScene::CreateGizmo()
{
	sceneGizmo = new Gizmo(this);
}

PhysicScene::PhysicScene() 
	: AdvancedScene()
{
	///collision configuration contains default setup for memory, collision setup
	sceneCollisionConfiguration = new btDefaultCollisionConfiguration();
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	scenePhysicsDispatcher = new	btCollisionDispatcher(sceneCollisionConfiguration);
	scenePhysicsBroadphase = new btDbvtBroadphase();
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	sceneSequentialImpulseConstraintSolver = new btSequentialImpulseConstraintSolver;

	physicsWorld = new btDiscreteDynamicsWorld(scenePhysicsDispatcher, scenePhysicsBroadphase, sceneSequentialImpulseConstraintSolver, sceneCollisionConfiguration);
	// On définit la gravité, de façon à ce que les objets tombent vers le bas (-Y).
	physicsWorld->setGravity(btVector3(0, 0, -10));

	GLDebugDrawer* WorldDebugDrawer = new GLDebugDrawer(this);
	WorldDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
	physicsWorld->setDebugDrawer(WorldDebugDrawer);
}
