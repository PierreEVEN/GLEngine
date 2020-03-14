#include "UI/StatWindow.h"
#include <imgui.h>
#include <CorePCH.h>

void StatWindow::AddValueToArray(const float& inValue, float* inArray, unsigned int elements, float& inMax) {
	if (elements > 0) {
		inMax = inValue;
		for (unsigned int pos = 1; pos < elements; ++pos) {
			if (inArray[pos] > inMax) inMax = inArray[pos];
			inArray[pos - 1] = inArray[pos];
		}
		inArray[elements - 1] = inValue;
	}
}

void StatWindow::DisplayPlots(float* values, const unsigned int& elements, const float& inMin, const float& inMax, std::string title, std::string description) {
	ImGui::PlotLines(title.data(), values, elements, 0, description.data(), inMin, inMax, ImVec2(0, 20));
}

StatWindow::StatWindow(std::string inWindowsTitle)
	: ImFloatingWindow(inWindowsTitle) {
	for (unsigned int i = 0; i < sizeof(GameThreadDeltaSecond) / sizeof(float); ++i) GameThreadDeltaSecond[i] = 0.0f;
	for (unsigned int i = 0; i < sizeof(RenderThreadDeltaSecond) / sizeof(float); ++i) RenderThreadDeltaSecond[i] = 0.0f;
}

void StatWindow::Draw(World* inWorld) {

	if (ImGui::Begin(windowTitle.data(), &bKeepOpen))
	{
		std::forward_list<std::string> StatGroups;
		for (auto& reader : *StatReader::GetReaders())
		{
			if ((std::find(StatGroups.begin(), StatGroups.end(), reader->GetGroupe()) == StatGroups.end()))	StatGroups.emplace_front(reader->GetGroupe());
		}
		for (const auto& name : StatGroups)
		{
			DrawGroup(name);
		}
		ImGui::End();
	}
}

void StatWindow::DrawGroup(const std::string& statGroup)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::Columns(1);
	ImGui::Text((" ---" + statGroup + "---").data());
	ImGui::Columns(2);
	ImGui::Separator();
	ImGui::Text("Name");
	ImGui::NextColumn();
	ImGui::Text("Value");
	ImGui::NextColumn();
	ImGui::Separator();

	for (auto& reader : *StatReader::GetReaders())
	{
		if (reader->GetGroupe() == statGroup)
		{
			ImGui::Text((reader->GetName() + " (" + std::to_string(reader->GetCallCount()) + ")").data());
			ImGui::NextColumn();
			if (reader->KeepHistory())
			{
				ImGui::PlotLines("", reader->GetValues(), reader->GetHistoryLength(), reader->GetOffset(), std::to_string(reader->GetAverage()).data(), reader->GetMin(), reader->GetMax(), ImVec2(0, 20));
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted(("Max : " + std::to_string(reader->GetMax())).data());
					ImGui::TextUnformatted(("Min : " + std::to_string(reader->GetMin())).data());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
			}
			else
			{
				ImGui::Text(std::string("(" + std::to_string(reader->GetCallCount()) + ")" + std::to_string((reader->GetValues()[0])) + "ms").data());
			}
			ImGui::NextColumn();
		}
	}
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::PopStyleVar();
	ImGui::Dummy(ImVec2(0.0f, 15.0f));
}
