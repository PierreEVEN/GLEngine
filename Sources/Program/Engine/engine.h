#pragma once

#define DISABLE_DOUBLE_BUFFERING true

struct GLFWwindow;

class Engine
{
public:

	static void Initialize();

private:

	static void StartOpenGL();
	static void CloseOpenGL();
	static void InitializeGlad();
	static void CreateOpenGLMainWindow();

	static void LoadEngineAssets();
	static void LoadGameAssets();

	static void InitializeImGui(GLFWwindow* parentWindow);
	static void CloseImGui();
	static void LoadEngineUILayout();

	static void CreatePrimaryWorld();

	static void AsyncLoop();
	static void StartGameThread();
};