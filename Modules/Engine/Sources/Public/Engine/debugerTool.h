#pragma once

#include <EnginePCH.h>
#include <UI/FloatingWindow.h>

#define ProfileStat(StatName) StatViewer newViewer = StatViewer(StatName)

#define ReadCurrentStat() (newViewer.GetCurrentDelay())

struct StatHistory
{
	StatHistory(const char* inStatName, double inStatDelay) : statName(inStatName), statDelay(inStatDelay) {}
	const char* statName = "";
	double statDelay = 0.0;
	int callCount = 1;
	void IncrementDelay(double inDelay)
	{
		statDelay += inDelay;
		callCount++;
	}

	bool operator==(const char* inStatName)
	{
		return inStatName == statName;
	}
};

struct StatViewer
{
	std::chrono::steady_clock::time_point startTime;
	const char* statName;
	StatViewer(const char* inStatName);
	virtual ~StatViewer();
	double GetCurrentDelay() const;
	static void AddDrawcall();
	static int GetDrawcalls();
	static void FlushStats();
	static void FlushDrawcallsCount();
};

struct StatReader
{
	float* Values = nullptr;
	float max;
	float min;
	float avg;
	double Lifetime = 0.0;
	double LastUseTime = 0.0;
	double UpdateInterval = -1.0;
	unsigned int HistoryLength = 1;
	unsigned int callCount = 0;
	std::string statName = "None";
	std::string statGroup = "Default";
	unsigned int CursorPosition = 0;

	std::thread::id myThread;

	void ClearValues();
	void Flush();
public:

	StatReader(std::string Name, std::string Group);
	~StatReader();

	FORCEINLINE bool IsPersistent() const { return Lifetime <= 0.0; }
	FORCEINLINE bool KeepHistory() const { return HistoryLength > 1; }

	static void RegisterStatParameters(std::string Name, std::string Group, double inLifetime = 2.0, unsigned int inHistoryPrecision = 1, double maxUpdateInterval = -1.0);
	static void AddStatValue(std::string Name, std::string Group, float value);
	static void FlushStats();




	bool ShouldBeDestroyed() const { return (!IsPersistent() && LastUseTime + Lifetime < glfwGetTime()); }
	float* GetValues() const { return Values; }
	unsigned int GetHistoryLength() const { return HistoryLength; }
	std::string GetName() const { return statName; }
	std::string GetGroupe() const { return statGroup; }
};

class StatWindow : public ImFloatingWindow
{
private:

	void UpdateData();

	void AddValueToArray(const float& inValue, float* inArray, unsigned int elements, float& inMax);

	void DisplayPlots(float* values, const unsigned int& elements, const float& inMax, std::string title, std::string description);

public:

	StatWindow(std::string inWindowsTitle);

	double RefreshRate = 0.1;
	double LastDeltaSecond = 0.0;
	float GameThreadDeltaSecondMax = 0.f;
	float RenderThreadDeltaSecondMax = 0.f;
	float GameThreadDeltaSecond[60];
	float RenderThreadDeltaSecond[60];

	virtual void Draw(World* inWorld) override;
};