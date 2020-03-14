#include <iostream>
#include <vector>
#include <Log/ProfilerTool.h>
#include <Log/LogSystem.h>

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
	StatReader::AddStatValue(statName, statGroup, (float)(GetCurrentDelay() * 1000.0));
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

void StatReader::CycleStat(const double& currentTime) {
	if (std::this_thread::get_id() != myThread) GLogAssert("Cannot cycle stat from other thread");
	StoreNewValue(CurrentFrameValue, currentTime);
	CurrentFrameValue = 0.f;
	LastFrameCallCount = CurrentCallCount;
	CurrentCallCount = 0;
}

void StatReader::StoreNewValue(const float& inValue, const double& currentTime) {
	if (UpdateInterval > 0.0 && LastUpdateTime + UpdateInterval > currentTime) return;
	LastUpdateTime = currentTime;
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

void StatReader::CycleStats(const double& currentTime) {
	for (auto& reader : ReaderMap) {
		if (std::this_thread::get_id() == reader->myThread) {
			reader->CycleStat(currentTime);
		}
	}
}

std::forward_list<StatReader*>* StatReader::GetReaders() {
	return &ReaderMap;
}

StatReader* StatReader::GetStatReader(const std::string& Name, const std::string& Group) {
	for (auto& reader : ReaderMap) {
		if (Name == reader->statName && Group == reader->statGroup && reader->myThread == std::this_thread::get_id()) return reader;
	}
	return new StatReader(Name, Group);
}
