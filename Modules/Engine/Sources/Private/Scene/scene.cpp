
#include <Camera/camera.h>
#include <Shader/material.h>
#include <Asset/AssetRegistry.h>
#include <Mesh/StaticMesh.h>
#include <World/world.h>
#include <Engine/inputManager.h>
#include <Scene/scene.h>
#include <Mesh/CubemapComponent.h>
#include <Physic/physicDebugViewer.h>

unsigned int lastGLScreenSizeX = 0, lastGLScreenSizeY = 0;

Scene::~Scene() 
{
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &textureColorbuffer);
	glDeleteRenderbuffers(1, &sceneRenderBufferObject);
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

GLDebugDrawer* Scene::CreateDebugDrawer()
{
	return new GLDebugDrawer(this);
}

void Scene::InitializeSceneGT()
{
	bHasBeenInitializedGT = true;
	SceneComponents = {};
	debugDrawer = CreateDebugDrawer();
	sceneCamera = CreateCamera();
}

void Scene::InitializeSceneRT()
{
	bHasBeenInitializedRT = true;
	defaultBackgroundColor = glm::vec4(DEFAULT_BACKGROUND_CLEAR_COLOR);
	framebuffer = -1;
	textureColorbuffer = -1;
	sceneRenderBufferObject = -1;
	bIsFramebufferValid = false;
	CreateFramebuffer();
}

Camera* Scene::CreateCamera()
{
	return new Camera(this, glm::vec3(0.f, 0.f, 0.f));
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
	glm::vec4 nearPos = glm::vec4(localPosX, localPosY, -1, 1.0);
	glm::vec4 farPos = glm::vec4(localPosX, localPosY, 1, 1.0);
	glm::vec4 nearResult = invMat * nearPos;
	glm::vec4 farResult = invMat * farPos;
	nearResult /= nearResult.w;
	farResult /= farResult.w;
	glm::vec3 dir = glm::vec3(farResult - nearResult);
	return glm::normalize(dir);
}

void Scene::RegisterComponent(SceneComponent* inComponent) {
	CHECK_GAME_THREAD;
	SceneComponents.emplace_front(inComponent);
}

void Scene::UnregisterComponent(SceneComponent* inComponent) {
	CHECK_GAME_THREAD;
	SceneComponents.remove(inComponent);
}

void Scene::RegisterSceneMeshProxy(SceneMeshProxy* inProxy) {
	CHECK_RENDER_THREAD;
	meshProxies.emplace_front(inProxy);
}

void Scene::UnregisterSceneMeshProxy(SceneMeshProxy* inProxy) {
	CHECK_RENDER_THREAD;
	meshProxies.remove(inProxy);
}

void Scene::SetDisplayMode(ESceneDebugDrawMode inMode) {
	debugDrawMode = inMode;
}

void Scene::Render() {
	CHECK_RENDER_THREAD;

	if (!bHasBeenInitializedRT) InitializeSceneRT();

	switch (debugDrawMode) {
	case ESceneDebugDrawMode::Lit: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case ESceneDebugDrawMode::Wireframe: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case ESceneDebugDrawMode::Points: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	if (lastGLScreenSizeX != sceneScreenWidth || lastGLScreenSizeY != sceneScreenHeight)
	{
		lastGLScreenSizeX = sceneScreenWidth;
		lastGLScreenSizeY = sceneScreenHeight;
		glViewport(0, 0, sceneScreenWidth, sceneScreenHeight);
	}

	glClearColor(defaultBackgroundColor.x, defaultBackgroundColor.y, defaultBackgroundColor.z, defaultBackgroundColor.w);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClear(GL_COLOR_BUFFER_BIT);

	Material::UpdateMaterialDefaults(this);

	for (auto& sceneComponent : SceneComponents)
	{
		if (sceneComponent->IsBeingDestroyed())
		{
			sceneComponent->DestroyOnRenderThread();
			continue;
		}
		else
		{
			sceneComponent->Render();
		}
	}

	/** Draw mesh proxies */
	for (int priorityIndex = 0; priorityIndex < DrawPriority::DrawPriority_Max; ++priorityIndex)
	{
		glClear(GL_DEPTH_BUFFER_BIT);

		for (auto& proxy : meshProxies)
		{
			if (proxy->GetDrawPriority() == priorityIndex)
			{
				if (proxy->IsVisible())
				{
					proxy->Draw();
				}
			}
		}
	}

	glDisable(GL_FRAMEBUFFER_SRGB);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::Tick(const double& inDeltaTime)
{
	CHECK_GAME_THREAD;

	if (!bHasBeenInitializedGT) InitializeSceneGT();

	for (auto& component : SceneComponents)
	{
		if (component->CanBeDeleted())
		{
			delete component;
		}
		else
		{
			component->Tick(inDeltaTime);
		}
	}
}

AdvancedScene::~AdvancedScene()
{
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
	if (!skybox) CreateCubemap();
	if (skybox) skybox->SetMaterial(0, cubemapMaterial);
}

void AdvancedScene::InitializeSceneGT()
{
	Scene::InitializeSceneGT();
	if (Material* SkyboxMaterial = AssetRegistry::FindAssetByName<Material>("CubemapMaterial")) {
		SetCubemapMaterial(SkyboxMaterial);
	}
}

void AdvancedScene::CreateCubemap()
{
	if (AStaticMesh* cubeMesh = AssetRegistry::FindAssetByName<AStaticMesh>("CubeMesh"))
	{
		cubeMesh->CheckData();
		skybox = new CubemapComponent(this);
		skybox->SetRotation(SRotator(270, 0, 0));
	}
	else
	{
		skybox = nullptr;
	}

}
