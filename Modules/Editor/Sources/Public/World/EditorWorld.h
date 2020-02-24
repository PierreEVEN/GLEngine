#pragma once
#include <World/world.h>


class EditorWorld : public World
{
public:

	EditorWorld(GLFWwindow* inWindow) : World(inWindow) {}


	virtual void TickWorld(double newDeltaSecond) override;

protected:

	void processInput();
	virtual Scene* CreateScene() override;
	virtual void DrawUI() override;
};