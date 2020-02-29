#include <iostream>
#include <imgui.h>
#include <Engine/debugerTool.h>
#include <vector>

std::vector<StatHistory> statHistory = {};
std::forward_list<StatReader*> ReaderMap = {};

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
			elem.IncrementDelay(GetCurrentDelay());
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

void StatReader::ClearValues()
{
	for (unsigned int i = 0; i < HistoryLength; ++i)
	{
		Values[i] = 0;
	}
}

void StatReader::Flush()
{

}

StatReader::StatReader(std::string Name, std::string Group)
	: statName(Name), statGroup(Group) {
	ReaderMap.emplace_front(this);
	myThread = std::this_thread::get_id();
}

StatReader::~StatReader()
{
	ReaderMap.remove(this);
}

void StatReader::RegisterStatParameters(std::string Name, std::string Group, double inLifetime, unsigned int inHistoryPrecision, double maxUpdateInterval)
{
	StatReader* foundReader = nullptr;
	for (auto& reader : ReaderMap)
	{
		if (Name == reader->statName && Group == reader->statGroup)
		{
			foundReader = reader;
		}
	}
	if (!foundReader) foundReader = new StatReader(Name, Group);

	foundReader->Lifetime = inLifetime;
	foundReader->LastUseTime = glfwGetTime();
	if (foundReader->HistoryLength != inHistoryPrecision)
	{
		if (foundReader->Values) free(foundReader->Values);
		foundReader->HistoryLength = inHistoryPrecision;
		foundReader->Values = (float*)malloc(sizeof(float) * foundReader->HistoryLength);
		foundReader->ClearValues();
	}
	foundReader->UpdateInterval = maxUpdateInterval;
}

void StatReader::AddStatValue(std::string Name, std::string Group, float value)
{
	StatReader* foundReader = nullptr;
	for (auto& reader : ReaderMap)
	{
		if (Name == reader->statName && Group == reader->statGroup && reader->myThread == std::this_thread::get_id())
		{
			foundReader = reader;
		}
	}
	if (foundReader && foundReader->UpdateInterval > 0.0 && foundReader->LastUseTime + foundReader->UpdateInterval > glfwGetTime()) return;

	if (!foundReader)
	{
		foundReader = new StatReader(Name, Group);
		std::cout << "created stat Reader" << std::endl;
	}

	foundReader->CursorPosition = (foundReader->CursorPosition + 1) % foundReader->HistoryLength;
	if (!foundReader->Values)
	{
		foundReader->Values = (float*)malloc(sizeof(float) * foundReader->HistoryLength);
		foundReader->ClearValues();
	}
	foundReader->Values[foundReader->CursorPosition] = value;
	foundReader->LastUseTime = glfwGetTime();
	std::cout << "Set value" << std::endl;
}

void StatReader::FlushStats()
{
	for (auto& reader : ReaderMap)
	{
		if (std::this_thread::get_id() == reader->myThread)
		{
			reader->Flush();
		}
	}
}

void StatWindow::UpdateData()
{
	if (glfwGetTime() < LastDeltaSecond + RefreshRate) return;
	LastDeltaSecond = glfwGetTime();

	AddValueToArray((float)ThreadHandler::GetGameThreadDeltaTime(), GameThreadDeltaSecond, sizeof(GameThreadDeltaSecond) / sizeof(float), GameThreadDeltaSecondMax);
	AddValueToArray((float)ThreadHandler::GetRenderThreadDeltaTime(), RenderThreadDeltaSecond, sizeof(RenderThreadDeltaSecond) / sizeof(float), RenderThreadDeltaSecondMax);
}

void StatWindow::AddValueToArray(const float& inValue, float* inArray, unsigned int elements, float& inMax)
{
	if (elements > 0)
	{
		inMax = inValue;
		for (unsigned int pos = 1; pos < elements; ++pos)
		{
			if (inArray[pos] > inMax) inMax = inArray[pos];
			inArray[pos - 1] = inArray[pos];
		}
		inArray[elements - 1] = inValue;
	}
}

void StatWindow::DisplayPlots(float* values, const unsigned int& elements, const float& inMax, std::string title, std::string description) {
	ImGui::PlotLines(title.data(), values, elements, 0, description.data(), 0.0, inMax, ImVec2(0, 20));
}

StatWindow::StatWindow(std::string inWindowsTitle)
	: ImFloatingWindow(inWindowsTitle) {
	for (unsigned int i = 0; i < sizeof(GameThreadDeltaSecond) / sizeof(float); ++i) GameThreadDeltaSecond[i] = 0.0f;
	for (unsigned int i = 0; i < sizeof(RenderThreadDeltaSecond) / sizeof(float); ++i) RenderThreadDeltaSecond[i] = 0.0f;
}

void StatWindow::Draw(World* inWorld) {

	UpdateData();
	if (ImGui::Begin(windowTitle.data(), &bKeepOpen))
	{
		DisplayPlots(GameThreadDeltaSecond, IM_ARRAYSIZE(GameThreadDeltaSecond), GameThreadDeltaSecondMax,
			std::string("GameThread - " + std::to_string((int)(1 / ThreadHandler::GetGameThreadDeltaTime())) + " fps "),
			std::string(std::to_string(ThreadHandler::GetGameThreadDeltaTime()) + " ms " + " ( max : " + std::to_string(GameThreadDeltaSecondMax) + " ms )"));
		DisplayPlots(RenderThreadDeltaSecond, IM_ARRAYSIZE(RenderThreadDeltaSecond), RenderThreadDeltaSecondMax,
			std::string("RenderThread - " + std::to_string((int)(1 / ThreadHandler::GetRenderThreadDeltaTime())) + " fps"),
			std::string(std::to_string(ThreadHandler::GetRenderThreadDeltaTime()) + " ms " + " ( max : " + std::to_string(RenderThreadDeltaSecondMax) + " ms )"));


		for (auto& reader : ReaderMap)
		{
			if (reader->KeepHistory())
			{
				DisplayPlots(reader->GetValues(), reader->GetHistoryLength(), 1.0, reader->GetName(), "");
			}
			else
			{
				ImGui::Text(std::string("(" + std::to_string(reader->callCount) + ")" + std::string(reader->GetName()) + " : " + std::to_string((reader->GetValues()[0])) + "ms").data());
			}
			if (reader->ShouldBeDestroyed())
			{
				std::cout << "delete reader" << std::endl;
				ReaderMap.remove(reader);
				delete reader;
			}
		}

// 		float barSize = 200;
// 		for (const auto& stat : statHistory)
// 		{
// 
// 			ImGui::ProgressBar((float)stat.statDelay / (1 / 30.f), ImVec2(barSize, 15));
// 			ImGui::SameLine(barSize + 20);
// 			if (stat.callCount != 1)
// 			{
// 				ImGui::Text(std::string("(" + std::to_string(stat.callCount) + ")" + std::string(stat.statName) + " : " + std::to_string((stat.statDelay * 1000)) + "ms").data());
// 			}
// 			else
// 			{
// 				ImGui::Text(std::string(std::string(stat.statName) + " : " + std::to_string((stat.statDelay * 1000)) + "ms").data());
// 			}
// 		}
		ImGui::End();
	}
}