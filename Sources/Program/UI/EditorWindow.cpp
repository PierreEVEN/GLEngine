#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_opengl3.h"
#include "../ImGUI/imgui_impl_glfw.h"
#include "EditorWindow.h"
#include "../World/world.h"
#include <string>
#include "../Camera/camera.h"
#include <glfw3/glfw3.h>
#include "../World/sceneComponent.h"
#include "../Lighting/directionalLight.h"
#include "../Lighting/pointLight.h"
#include "../Lighting/spotLight.h"
#include "../Asset/assetLibrary.h"
#include <Windows.h>
#include "../Asset/AssetImporter.h"
#include "../Texture/texture.h"
#include "../Asset/AssetRegistry.h"
#include "../Mesh/staticMeshComponent.h"
#include "../Shader/material.h"
#include "Importers/importerWindows.h"
#include "EditorWindows/assetEditor.h"
#include "../Mesh/staticMesh.h"

std::vector<UIWindowElement*> WindowManager::elementsArray;

static bool bShowTextures = false;
static bool bShowMaterials = false;
static bool bShowMesh = true;

static float MAX_FRAMERATE = 60.f;

void EditorWindow::DrawWindow(World* InWorld)
{
	DrawMainToolbar(InWorld);
	WindowManager::DrawElements(InWorld);

	ImGui::Begin("Content browser");
	ImGui::BeginGroup();

	ImGui::Checkbox("show materials ", &bShowMaterials);
	ImGui::Checkbox("show meshes ", &bShowMesh);
	ImGui::Checkbox("show textures ", &bShowTextures);

	for (auto& asset : AssetRegistry::GetAssets())
	{
		if (bShowMaterials)
		{
			if (Material* foundMat = dynamic_cast<Material*>(asset))
			{
				ImGui::Button(asset->GetName().data());
			}
		}
		if (bShowTextures)
		{
			if (Texture2D* foundText = dynamic_cast<Texture2D*>(asset))
			{
				ImGui::Button(asset->GetName().data());
			}
		}
		if (bShowMesh)
		{
			if (StaticMesh* foundMesh = dynamic_cast<StaticMesh*>(asset))
			{
				if (ImGui::Button(asset->GetName().data()))
				{
					new StaticMeshEditorWindows(foundMesh);
					StaticMeshComponent* newComp = new StaticMeshComponent(InWorld, foundMesh, {});
					newComp->SetAngle(90.f);
					newComp->SetScale3D(SVector3(10.f));
				}
			}
		}
	}
	ImGui::EndGroup();
	ImGui::End();
}

double EditorWindow::GetMaxFramerate()
{
	return MAX_FRAMERATE;
}

void EditorWindow::DrawMainToolbar(World* InWorld)
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
	{
		ImGui::Button("OpenWorld");
		ImGui::Button("SaveWorld");
		if (ImGui::Button("Quit")) { glfwSetWindowShouldClose(InWorld->GetWindow(), true); }
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit"))
	{
		ImGui::Button("Edit texture");
		ImGui::Button("Edit material");
		ImGui::Button("Edit mesh");
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Import"))
	{
		if (ImGui::Button("Import Texture"))
		{
			new TextureImporterWindow();
		}
		if (ImGui::Button("Import material"))
		{
			new ShaderImporterWindow();
		}
		if (ImGui::Button("Import mesh"))
		{
			new MeshImporterWindow();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::BeginMenu("Shading"))
		{
			if (ImGui::Button("Lit")) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
			if (ImGui::Button("Wireframe")) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
			if (ImGui::Button("Vertices")) { glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); }
			ImGui::EndMenu();
		}
		ImGui::Checkbox("Skip light rendering", &InWorld->bSkipLightRendering);
		ImGui::Checkbox("Simulate physics", &InWorld->bSimulatePhysics);
		ImGui::SliderFloat("Camera velocity", &InWorld->GetCamera()->MovementSpeed, 1, 2000, "%3.f", 4.f);
		ImGui::SliderFloat("Field of view", &InWorld->GetCamera()->Zoom, 10, 140, "%3.f", 1.f);
		ImGui::SliderFloat("Max framerate", &MAX_FRAMERATE, 20, 5000, "%3.f", 1.f);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Info"))
	{
		if (ImGui::BeginMenu(std::string("Scene components : " + std::to_string(InWorld->GetSceneComponents().size())).data()))
		{
			int Counter = 0;
			for (auto& component : InWorld->GetSceneComponents())
			{
				Counter++;
				if (ImGui::Button(std::string("SceneComponent_" + std::to_string(Counter)).data()))
				{
					InWorld->GetCamera()->SetCameraLocation(component->GetLocation() - InWorld->GetCamera()->GetCameraForwardVector() * 4.f);
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(std::string("Point light : " + std::to_string(InWorld->GetPointLightSources().size())).data()))
		{
			int Counter = 0;
			for (auto& component : InWorld->GetPointLightSources())
			{
				Counter++;
				if (ImGui::Button(std::string("PointLight_" + std::to_string(Counter)).data()))
				{
					InWorld->GetCamera()->SetCameraLocation(component->GetLocation() - InWorld->GetCamera()->GetCameraForwardVector() * 4.f);
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(std::string("Directional light : " + std::to_string(InWorld->GetDirectionalLightSources().size())).data()))
		{
			int Counter = 0;
			for (auto& component : InWorld->GetDirectionalLightSources())
			{
				Counter++;
				if (ImGui::Button(std::string("DirectionalLight_" + std::to_string(Counter)).data()))
				{
					InWorld->GetCamera()->SetCameraLocation(component->GetLocation() - InWorld->GetCamera()->GetCameraForwardVector() * 4.f);
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(std::string("Spot light : " + std::to_string(InWorld->GetSpotLightSources().size())).data()))
		{
			int Counter = 0;
			for (auto& component : InWorld->GetSpotLightSources())
			{
				Counter++;
				if (ImGui::Button(std::string("SpotLight_" + std::to_string(Counter)).data()))
				{
					InWorld->GetCamera()->SetCameraLocation(component->GetLocation() - InWorld->GetCamera()->GetCameraForwardVector() * 4.f);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::Text(std::string().data());
		ImGui::EndMenu();
	}
	ImGui::AlignTextToFramePadding();
	ImGui::Indent(ImGui::GetWindowWidth() - 130.0f);
	ImGui::Text(std::string("fps : " + std::to_string(1.0 / InWorld->GetWorldDeltaSecond())).data());
	ImGui::EndMainMenuBar();
}


void WindowManager::DrawElements(World* inWorld)
{
	for (int i = WindowManager::elementsArray.size() - 1; i >= 0; --i)
	{
		WindowManager::elementsArray[i]->Draw(inWorld);
		if (!WindowManager::elementsArray[i]->bKeepOpen)
		{
			CloseWindow(elementsArray[i]);
		}
	}
}

void WindowManager::AddWindow(UIWindowElement* inElement)
{
	WindowManager::elementsArray.push_back(inElement);
}

void WindowManager::CloseWindow(UIWindowElement* inElement)
{
	for (unsigned int i = 0; i < WindowManager::elementsArray.size(); ++i)
	{
		if (WindowManager::elementsArray[i] == inElement)
		{
			delete inElement;
			WindowManager::elementsArray.erase(WindowManager::elementsArray.begin() + i);
			return;
		}
	}
}

UIWindowElement::UIWindowElement()
{
	WindowManager::AddWindow(this);
}
