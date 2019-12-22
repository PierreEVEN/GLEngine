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
#include "ContentBrowser/contentBrowser.h"
#include "EditorWindows/engineLogWindow.h"
#include "../EngineLog/engineLog.h"
#include <bullet3D/LinearMath/btIDebugDraw.h>
#include <bullet3D/btBulletDynamicsCommon.h>
#include "../Engine/debugerTool.h"

std::vector<UIWindowElement*> WindowManager::elementsArray;

bool bShowDemoWindow;
bool bDebugPhysics = false;

static float MAX_FRAMERATE = 60.f;

void EditorWindow::DrawWindow(World* InWorld, float& ViewportLocationX, float& ViewportLocationY)
{
	if (bShowDemoWindow)
	{
		ImGui::ShowDemoWindow(&bShowDemoWindow);
	}
	DrawMainToolbar(InWorld);
	DrawBackgroundDockspace(InWorld);
	WindowManager::DrawElements(InWorld);


	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport", 0, ImGuiWindowFlags_NoScrollbar);
	ImGui::PopStyleVar();
	if (ImGui::GetWindowWidth() != InWorld->GetScreenWidth() || ImGui::GetWindowHeight() != InWorld->GetScreenHeight())
	{
		InWorld->UpdateFramebufferSize((int)ImGui::GetWindowWidth(), (int)ImGui::GetWindowHeight());
	}
	ImGui::Image((void*)(intptr_t)InWorld->textureColorbuffer, ImVec2((float)InWorld->GetScreenWidth(), (float)InWorld->GetScreenHeight()), ImVec2(0, 1), ImVec2(1, 0));
	ImVec2 windowsPos = ImGui::GetWindowPos();
	ViewportLocationX = windowsPos.x;
	ViewportLocationY = windowsPos.y;

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DDOP_ASSET"))
		{
			if (StaticMesh* foundMesh = AssetRegistry::FindAssetByID<StaticMesh>(*(unsigned long*)payload->Data))
			{
				StaticMeshComponent* spawnedMeshComp = new StaticMeshComponent(InWorld, foundMesh);
				spawnedMeshComp->SetAngle(90.f);
				spawnedMeshComp->SetScale3D(SVector3(10.f));
			}
		}
		ImGui::EndDragDropTarget();
	}
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
		if (ImGui::Button("Quit")) { glfwSetWindowShouldClose(InWorld->GetWindow(), true); }
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Window"))
	{
		if (ImGui::Button("New content browser"))
		{
			new ContentBrowser("Content browser");
		}
		if (ImGui::Button("Console"))
		{
			new EngineLogWindow("Console");
		}
		if (ImGui::Button("Debuger"))
		{
			new StatWindow("Debuger");
		}
		if (ImGui::Button("Open demo window"))
		{
			bShowDemoWindow = true;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Import"))
	{
		if (ImGui::Button("Import Texture"))
		{
			new TextureImporterWindow("Texture importer");
		}
		if (ImGui::Button("Import material"))
		{
			new ShaderImporterWindow("Material importer");
		}
		if (ImGui::Button("Import mesh"))
		{
			new MeshImporterWindow("Mesh importer");
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
		ImGui::Checkbox("Debug physics", &bDebugPhysics);
		InWorld->GetPhysicWorld()->getDebugDrawer()->setDebugMode(bDebugPhysics ? btIDebugDraw::DBG_DrawWireframe : btIDebugDraw::DBG_NoDebug);
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
	ImGui::Indent(ImGui::GetWindowWidth() - 320.0f);
	ImGui::Text(std::string(std::to_string(InWorld->GetSceneComponents().size()) + " components | " + std::to_string(StatViewer::GetDrawcalls()) + " Drawcalls | " + "fps : " + std::to_string((int)(1.0 / InWorld->GetWorldDeltaSecond()))).data());
	ImGui::EndMainMenuBar();
}

void EditorWindow::DrawBackgroundDockspace(World* InWorld)
{
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", 0, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Docking"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
			if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
			ImGui::Separator();
// 			if (ImGui::MenuItem("Close DockSpace", NULL, false, p_open != NULL))
// 				*p_open = false;
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

std::string WindowManager::GetUnexistingWindowName(std::string inWindowName)
{
	int currentIndex = 0;
	bool bFoundItem = false;
	for (const auto& element : elementsArray)
	{
		std::string CurrentWindowName = inWindowName + (bFoundItem ? std::to_string(currentIndex) : "");
		if (element->windowTitle == CurrentWindowName)
		{
			bFoundItem = true;
			currentIndex++;
		}
	}
	return bFoundItem ? inWindowName + std::to_string(currentIndex) : inWindowName;
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

UIWindowElement::UIWindowElement(std::string inWindowTitle)
{
	windowTitle = WindowManager::GetUnexistingWindowName(inWindowTitle);
	WindowManager::AddWindow(this);
}