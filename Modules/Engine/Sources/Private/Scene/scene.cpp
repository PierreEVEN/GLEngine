
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
	for (auto& component : components)
	{
		component->DestroyComponent();
	}
	for (int i = components.size() - 1; i >= 0; --i)
	{
		delete components[i];
	}
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

void Scene::InitializeScene()
{
	defaultBackgroundColor = glm::vec4(DEFAULT_BACKGROUND_CLEAR_COLOR);
	components = {};
	framebuffer = -1;
	textureColorbuffer = -1;
	sceneRenderBufferObject = -1;
	bIsFramebufferValid = false;
	CreateFramebuffer();
	debugDrawer = CreateDebugDrawer();
	sceneCamera = CreateCamera();
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

void Scene::SetDisplayMode(ESceneDebugDrawMode inMode)
{
	switch (inMode)
	{
	case ESceneDebugDrawMode::Lit: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case ESceneDebugDrawMode::Wireframe: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case ESceneDebugDrawMode::Points: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
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
	glClear(GL_COLOR_BUFFER_BIT);

	Material::UpdateMaterialDefaults(this);
	for (int i = 0; i < DrawPriority::DrawPriority_Max; ++i)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		for (SceneComponentIterator<SceneComponent> ite(this); ite; ite++)
		{
			if ((*ite)->drawPriority == i)
			{
				if (!(*ite)->IsBeingDestroyed())
				{
					(*ite)->Tick();
				}
			}
		}
		if (i == DrawPriority::DrawPriority_Normal)
		{
			GetDebugDrawer()->RenderDebugDraw();
		}

		for (int i = components.size() - 1; i >= 0; --i)
		{
			if (components[i]->IsBeingDestroyed())
			{
				delete components[i];
			}
		}
	}
	glDisable(GL_FRAMEBUFFER_SRGB);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

AdvancedScene::~AdvancedScene()
{
	if (skybox)
	{
		skybox->DestroyComponent();
		delete skybox;
	}
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
	if (skybox) skybox->materialOverride = cubemapMaterial;
}

void AdvancedScene::InitializeScene()
{
	Scene::InitializeScene();
	if (Material* SkyboxMaterial = AssetRegistry::FindAssetByName<Material>("CubemapMaterial")) {
		SetCubemapMaterial(SkyboxMaterial);
	}
}

void AdvancedScene::CreateCubemap()
{
	if (AStaticMesh* cubeMesh = AssetRegistry::FindAssetByName<AStaticMesh>("CubeMesh"))
	{
		cubeMesh->CheckData();
		skybox = new CubemapComponent(this, nullptr, cubeMesh->GetSections()[0]);
		skybox->SetRotation(SRotator(270, 0, 0));
	}
	else
	{
		skybox = nullptr;
	}

}
