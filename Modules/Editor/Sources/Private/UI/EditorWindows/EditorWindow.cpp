
#include <imgui.h>
#include <World/world.h>
#include <Mesh/staticMeshComponent.h>
#include <Asset/AssetRegistry.h>
#include <MathLibrary/vector3.h>
#include <Camera/camera.h>
#include <Asset/GLAssetIO.h>
#include <Engine/debugerTool.h>
#include <Mesh/StaticMesh.h>
#include <UI/EditorWindows/EditorWindow.h>
#include <UI/AssetImporterWindow.h>
#include <UI/engineLogWindow.h>
#include <UI/EditorWindows/contentBrowser.h>
#include <Scene/scene.h>
#include <Engine/engine.h>

std::vector<ImFloatingWindow*> WindowManager::elementsArray;

bool bShowDemoWindow = false;
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
	if ((int)ImGui::GetWindowWidth() != (int)InWorld->GetScene()->GetScreenWidth() || (int)ImGui::GetWindowHeight() != (int)InWorld->GetScene()->GetScreenHeight())
	{
		InWorld->GetScene()->UpdateFramebufferSize((int)ImGui::GetWindowWidth(), (int)ImGui::GetWindowHeight());
		((AdvancedScene*)InWorld->GetScene())->SetSceneWindowLocation((int)ImGui::GetWindowPos().x, (int)ImGui::GetWindowPos().y);
	}
	ImGui::Image((void*)(intptr_t)InWorld->GetScene()->GetColorBuffer(), ImVec2((float)InWorld->GetScene()->GetScreenWidth(), (float)InWorld->GetScene()->GetScreenHeight()), ImVec2(0, 1), ImVec2(1, 0));
	ImVec2 windowsPos = ImGui::GetWindowPos();
	ViewportLocationX = windowsPos.x;
	ViewportLocationY = windowsPos.y;

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DDOP_ASSET"))
		{
			if (AStaticMesh* foundMesh = AssetRegistry::FindAssetByID<AStaticMesh>(*(unsigned long*)payload->Data))
			{
				StaticMeshComponent* spawnedMeshComp = new StaticMeshComponent(InWorld->GetScene(), foundMesh, {});
				spawnedMeshComp->SetRotation(SRotator(90, 0, 0));
				spawnedMeshComp->SetScale3D(SVector3(1.f));
				spawnedMeshComp->SetLocation(InWorld->GetScene()->GetCamera()->GetLocation() + InWorld->GetScene()->GetCamera()->GetRotation().GetForwardVector() * 10);
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
		if (ImGui::Button("Save All")) { GLAssetIO::SaveUnsavedAsset(); }
		if (ImGui::Button("Quit")) { Engine::ShutDown(); }
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
		if (ImGui::Button("Import Texture2D"))
		{
			new TextureImporterWindow("Texture importer");
		}
		if (ImGui::Button("Import TextureCube"))
		{
			new TextureCubeImporterWindow("Texture cubemap importer");
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
		//ImGui::Checkbox("Simulate physics", &InWorld->bSimulatePhysics);
		ImGui::Checkbox("Debug physics", &bDebugPhysics);
		bool bDrawDebugBox = PrimitiveComponent::DoesDrawBoxBounds();
		bool bDrawDebugSphere = PrimitiveComponent::DoesDrawSphereBounds();
		ImGui::Checkbox("Draw box bounds", &bDrawDebugBox);
		ImGui::Checkbox("Draw sphere bounds", &bDrawDebugSphere);
		if (bDrawDebugBox != PrimitiveComponent::DoesDrawBoxBounds()) PrimitiveComponent::SetDrawBoxBounds(bDrawDebugBox);
		if (bDrawDebugSphere != PrimitiveComponent::DoesDrawSphereBounds()) PrimitiveComponent::SetDrawSphereBounds(bDrawDebugSphere);
		ImGui::SliderFloat("Camera velocity", &InWorld->GetScene()->GetCamera()->MovementSpeed, 1, 2000, "%3.f", 4.f);
		ImGui::SliderFloat("Field of view", &InWorld->GetScene()->GetCamera()->Zoom, 10, 140, "%3.f", 1.f);
		ImGui::SliderFloat("Max framerate", &MAX_FRAMERATE, 20, 5000, "%3.f", 1.f);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Info"))
	{
		ImGui::Text(std::string().data());
		ImGui::EndMenu();
	}
	ImGui::AlignTextToFramePadding();
	ImGui::Indent(ImGui::GetWindowWidth() - 320.0f);
	ImGui::Text(std::string(std::to_string(StatViewer::GetDrawcalls()) + " Drawcalls | " + "fps : " + std::to_string((int)(1.0 / InWorld->GetWorldDeltaSecond()))).data());
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

void WindowManager::AddWindow(ImFloatingWindow* inElement)
{
	WindowManager::elementsArray.push_back(inElement);
}

void WindowManager::CloseWindow(ImFloatingWindow* inElement)
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

ImFloatingWindow::ImFloatingWindow(std::string inWindowTitle)
{
	windowTitle = WindowManager::GetUnexistingWindowName(inWindowTitle);
	WindowManager::AddWindow(this);
}