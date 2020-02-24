
#pragma once
#include <Engine/engine.h>

class EditorEngine : public Engine
{

public:
    
    EditorEngine() : Engine() {}

protected:

	virtual void LoadAssets() override;
	virtual void CreateUI() override;
	virtual void CreateWorld(GLFWwindow* inWindow) override;


};