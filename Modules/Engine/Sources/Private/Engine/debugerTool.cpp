#include <iostream>
#include <imgui.h>
#include <Engine/debugerTool.h>
#include <vector>

std::forward_list<StatReader*> ReaderMap = {};

int drawCallsCount = 0;
int currentDrawCalls = 0;

StatCycleTimer::StatCycleTimer(const char* inStatName, const char* inStatGroup)
	: statName(inStatName), statGroup(inStatGroup)
{
	startTime = std::chrono::high_resolution_clock::now();
}

StatCycleTimer::~StatCycleTimer()
{
	StatReader::AddStatValue(statName, statGroup, GetCurrentDelay() * 1000.0);
}

double StatCycleTimer::GetCurrentDelay() const {
	return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startTime).count();
}


StatReader::StatReader(std::string Name, std::string Group, unsigned int inHistoryPrecision, double inMaxUpdateInterval) {
	SetStatParameters(Name, Group, inHistoryPrecision, inMaxUpdateInterval);
	ReaderMap.emplace_front(this);
	myThread = std::this_thread::get_id();
	LastUpdateTime = 0.0;
	if (!Values) Values = (float*)malloc(sizeof(float) * HistoryLength);
}

StatReader::~StatReader() {}

void StatReader::ClearValues() {
	for (unsigned int i = 0; i < HistoryLength; ++i)
	{
		Values[i] = 0;
	}
}

void StatReader::CycleStat() {
	if (std::this_thread::get_id() != myThread) GLogAssert("Cannot cycle stat from other thread");
	StoreNewValue(CurrentFrameValue);
	CurrentFrameValue = 0.f;
	LastFrameCallCount = CurrentCallCount;
	CurrentCallCount = 0;
}

void StatReader::StoreNewValue(const float& inValue) {
	if (UpdateInterval > 0.0 && LastUpdateTime + UpdateInterval > glfwGetTime()) return;
	LastUpdateTime = glfwGetTime();
	CursorPosition = (CursorPosition + 1) % HistoryLength;
	if (!Values) {
		Values = (float*)malloc(sizeof(float) * HistoryLength);
		ClearValues();
	}
	Values[CursorPosition] = inValue;
	maxValue = inValue;
	minValue = inValue;
	avgValue = 0;
	for (unsigned int i = 0; i < HistoryLength; ++i)
	{
		if (Values[i] > maxValue) maxValue = Values[i];
		if (Values[i] < minValue) minValue = Values[i];
		avgValue += Values[i];
	}
	avgValue /= HistoryLength;
}

void StatReader::SetStatParameters(std::string Name, std::string Group, unsigned int inHistoryPrecision, double maxUpdateInterval) {
	statName = Name;
	statGroup = Group;
	if (HistoryLength != inHistoryPrecision) {
		if (Values) free(Values);
		HistoryLength = inHistoryPrecision;
		Values = (float*)malloc(sizeof(float) * HistoryLength);
		ClearValues();
	}
	UpdateInterval = maxUpdateInterval;
}

void StatReader::AddStatValue(std::string Name, std::string Group, float value) {
	StatReader* foundReader = GetStatReader(Name, Group);
	foundReader->CurrentFrameValue += value;
	foundReader->CurrentCallCount++;
}

void StatReader::CycleStats() {
	for (auto& reader : ReaderMap) {
		if (std::this_thread::get_id() == reader->myThread) {
			reader->CycleStat();
		}
	}
}

StatReader* StatReader::GetStatReader(const std::string& Name, const std::string& Group) {
	for (auto& reader : ReaderMap) {
		if (Name == reader->statName && Group == reader->statGroup && reader->myThread == std::this_thread::get_id()) return reader;
	}
	return new StatReader(Name, Group);
}

void StatWindow::UpdateData() {
	if (glfwGetTime() < LastDeltaSecond + RefreshRate) return;
	LastDeltaSecond = glfwGetTime();

	AddValueToArray((float)ThreadHandler::GetGameThreadDeltaTime(), GameThreadDeltaSecond, sizeof(GameThreadDeltaSecond) / sizeof(float), GameThreadDeltaSecondMax);
	AddValueToArray((float)ThreadHandler::GetRenderThreadDeltaTime(), RenderThreadDeltaSecond, sizeof(RenderThreadDeltaSecond) / sizeof(float), RenderThreadDeltaSecondMax);
}

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

	UpdateData();
	if (ImGui::Begin(windowTitle.data(), &bKeepOpen))
	{
		std::forward_list<std::string> StatGroups;
		for (auto& reader : ReaderMap)
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

	for (auto& reader : ReaderMap)
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
