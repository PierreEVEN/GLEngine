#include <chrono>
#include <ctime>
#include "../UI/EditorWindow.h"

#define ProfileStat(StatName) StatViewer newViewer = StatViewer(StatName)

#define ReadCurrentStat() (newViewer.GetCurrentDelay())

struct StatHistory
{
	StatHistory(const char* inStatName, double inStatDelay) : statName(inStatName), statDelay(inStatDelay) {}
	const char* statName = "";
	double statDelay = 0.0;

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
	~StatViewer();
	double GetCurrentDelay() const;
	static void FlushStats();
};

class StatWindow : public UIWindowElement
{
public:
	StatWindow(std::string inWindowsName) : UIWindowElement(inWindowsName) {}
	virtual void Draw(World* inWorld) override;
};