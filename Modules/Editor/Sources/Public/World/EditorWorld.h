#pragma once
#include <World/world.h>


class EditorWorld : public World
{
public:

	EditorWorld(GLFWwindow* inWindow) : World(inWindow) {}


	virtual void Tick(double newDeltaSecond) override;

	virtual void Initialize() override;
private:

	void PopCube();

protected:

	void processInput();
	virtual Scene* CreateScene() override;
	virtual void DrawUI() override;
};