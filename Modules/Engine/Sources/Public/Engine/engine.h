#pragma once
#include <thread>

#define DISABLE_DOUBLE_BUFFERING true

struct GLFWwindow;

class Engine
{
public:
	
	static Engine* Get();

	static void Start();
	static void ShutDown();
	static std::thread::id GetGameThreadID();

protected:

	Engine();
	virtual ~Engine();

	virtual void PreInitialize() {}
	virtual void PostInitialize() {}

	virtual void LoadAssets() {}
	virtual void CreateUI() {}
	virtual void CreateWorld(GLFWwindow* inWindow) = 0;
	virtual void PreShutdown() {}
	virtual void PostShutdown() {}

private:

	bool bRequestClose = false;


	void Initialize();

	void StartOpenGL();
	void CloseOpenGL();
	void InitializeGlad();
	void CreateOpenGLMainWindow();

	void InitializeImGui(GLFWwindow* parentWindow);
	void CloseImGui();

	void CreatePrimaryWorld();

	void AsyncLoop();
	void StartGameThread();
};