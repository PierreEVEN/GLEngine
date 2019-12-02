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

void EditorWindow::DrawWindow(World* InWorld)
{
	DrawMainToolbar(InWorld);


	ImGui::Begin("Content browser");
	ImGui::BeginGroup();
	for (auto& asset : AssetLibrary::GetAssetRegistry())
	{
		if (ImGui::Button(asset->GetName().data()))
		{
			std::cout << asset->GetProperty("TextureData")->propertyValue << std::endl;
		}
	}
	ImGui::EndGroup();
	ImGui::End();
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
		if (ImGui::BeginMenu("Import texture"))
		{
			DrawTexture2DImporterWindow();
			ImGui::EndMenu();
		}
		ImGui::Button("Import material");
		ImGui::Button("Import mesh");
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

void EditorWindow::DrawContentBrowser(std::string directoryPath)
{
	std::string search_path = directoryPath + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (std::string(fd.cFileName) != std::string("."))
				{
					if (ImGui::BeginMenu(fd.cFileName))
					{
						DrawContentBrowser(directoryPath + "/" + fd.cFileName);
						ImGui::EndMenu();
					}
				}
			}
			else
			{
				if (ImGui::Button(fd.cFileName))
				{
					AssetImporter::ImportTexture(directoryPath + "/" + fd.cFileName, "TestTexture", "GLEngine/Sources/Assets/Textures/TestTexture.GLAsset");
				}
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
}

void EditorWindow::DrawTexture2DImporterWindow()
{
	DrawContentBrowser("./");
}
