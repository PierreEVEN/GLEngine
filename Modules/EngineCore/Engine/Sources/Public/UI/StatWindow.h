#pragma once
#include "FloatingWindow.h"


class StatWindow : public ImFloatingWindow
{
private:

	void UpdateData();

	void AddValueToArray(const float& inValue, float* inArray, unsigned int elements, float& inMax);

	void DisplayPlots(float* values, const unsigned int& elements, const float& inMin, const float& inMax, std::string title, std::string description);

	void DrawGroup(const std::string& statGroup);

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