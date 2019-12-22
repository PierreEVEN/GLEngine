#include "debugerTool.h"
#include "../EngineLog/engineLog.h"
#include <iostream>
#include "../ImGUI/imgui.h"

std::vector<StatHistory> statHistory = {};
int drawCallsCount = 0;
int currentDrawCalls = 0;

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

void StatViewer::AddDrawcall()
{
	currentDrawCalls++;
}

int StatViewer::GetDrawcalls()
{
	return drawCallsCount;
}

void StatViewer::FlushStats()
{
	statHistory.clear();
}

void StatViewer::FlushDrawcallsCount()
{
	drawCallsCount = currentDrawCalls;
	currentDrawCalls = 0;
}

void StatWindow::Draw(World* inWorld)
{
	ImGui::Begin(windowTitle.data(), &bKeepOpen);
	float barSize = 200;
	for (const auto& stat : statHistory)
	{
		ImGui::ProgressBar((float)stat.statDelay / (1 / 30.f), ImVec2(barSize, 15));
		ImGui::SameLine(barSize + 20);
		ImGui::Text(std::string(std::string(stat.statName) + " : " + std::to_string((stat.statDelay * 1000)) + "ms").data());
	}
	ImGui::End();
}