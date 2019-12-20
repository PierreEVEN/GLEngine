#include "debugerTool.h"
#include "../EngineLog/engineLog.h"
#include <iostream>
#include "../ImGUI/imgui.h"

std::vector<StatHistory> statHistory;

StatViewer::StatViewer(const char* inStatName)
	: statName(inStatName)
{
	startTime = std::chrono::high_resolution_clock::now();
}

StatViewer::~StatViewer()
{
	for(auto& elem : statHistory)
	{
		if (elem == statName)
		{
			elem.statDelay += GetCurrentDelay();
			return;
		}
	}
	statHistory.push_back(StatHistory(statName, GetCurrentDelay()));
}

double StatViewer::GetCurrentDelay() const
{
	return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startTime).count();
}

void StatViewer::FlushStats()
{
	statHistory.clear();
}

void StatWindow::Draw(World* inWorld)
{
	ImGui::Begin(windowTitle.data(), &bKeepOpen);
	for (const auto& stat : statHistory)
	{
		ImGui::Text(std::string(std::string(stat.statName) + " : " + std::to_string((stat.statDelay)) + "ms").data());
	}
	ImGui::End();
}
