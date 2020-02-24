#pragma once

#include <chrono>
#include <ctime>
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

class StatWindow : public ImFloatingWindow
{
public:
	StatWindow(std::string inWindowsName) : ImFloatingWindow(inWindowsName) {}
	virtual void Draw(World* inWorld) override;
};