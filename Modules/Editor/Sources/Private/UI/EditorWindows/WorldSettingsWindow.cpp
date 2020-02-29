#include <UI/EditorWindows/WorldSettingsWindow.h>
#include <imgui.h>

WorldSettingsWindow::WorldSettingsWindow(std::string windowTitle, World* inParentWorld)
	: ImFloatingWindow(windowTitle) {

	parentWorld = inParentWorld;
}

void WorldSettingsWindow::Draw(World* inWorld)
{
	ImGui::Begin(windowTitle.data(), &bKeepOpen);
	float barSize = 200;
	ImGui::End();
}