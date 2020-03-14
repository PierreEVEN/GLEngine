#pragma once

#include <EngineDefaults.h>

#define ProfileStat(StatName, StatGroup) StatCycleTimer newViewer = StatCycleTimer(StatName, StatGroup)

#define ReadCurrentStat() (newViewer.GetCurrentDelay())

struct StatCycleTimer
{
	std::chrono::steady_clock::time_point startTime;
	const char* statName;
	const char* statGroup;
	StatCycleTimer(const char* inStatName, const char* inStatGroup);
	virtual ~StatCycleTimer();
	double GetCurrentDelay() const;
};

class StatReader
{
private:
	float* Values = nullptr;
	float maxValue;
	float minValue;
	float avgValue;
	float CurrentFrameValue = 0.f;
	double LastUpdateTime = 0.0;
	double UpdateInterval = -1.0;
	unsigned int HistoryLength = 1;
	unsigned int CurrentCallCount = 0;
	unsigned int LastFrameCallCount = 0;
	std::string statName = "None";
	std::string statGroup = "Default";
	unsigned int CursorPosition = 0;

	std::thread::id myThread;

	void ClearValues();
	void CycleStat(const double& currentTime);
	void StoreNewValue(const float& inValue, const double& currentTime);

public:

	StatReader(std::string Name, std::string Group, unsigned int inHistoryPrecision = 60, double inMaxUpdateInterval = 1/60.0);
	~StatReader();

	FORCEINLINE bool KeepHistory() const { return HistoryLength > 1; }

	void SetStatParameters(std::string Name, std::string Group, unsigned int inHistoryPrecision = 1, double maxUpdateInterval = -1.0);
	void SetStatParameters(unsigned int inHistoryPrecision = 1, double maxUpdateInterval = -1.0) { SetStatParameters(statName, statGroup, inHistoryPrecision, maxUpdateInterval); }
	static void AddStatValue(std::string Name, std::string Group, float value);
	static void CycleStats(const double& currentTime);
	static std::forward_list<StatReader*>* GetReaders();
	static StatReader* GetStatReader(const std::string& Name, const std::string& Group);


	float* GetValues() const { return Values; }
	float GetMin() const { return minValue; }
	float GetMax() const { return maxValue; }
	float GetAverage() const { return avgValue; }
	unsigned int GetOffset() const { return CursorPosition; }
	unsigned int GetHistoryLength() const { return HistoryLength; }
	unsigned int GetCallCount() const { return LastFrameCallCount; }
	std::string GetName() const { return statName; }
	std::string GetGroupe() const { return statGroup; }
};
