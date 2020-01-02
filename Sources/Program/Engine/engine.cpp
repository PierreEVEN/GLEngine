#include "engine.h"
#include <glad/glad.h>
#include <glfw3/glfw3.h>
#include <thread>

#include "../World/world.h"
#include "../Asset/AssetRegistry.h"
#include "../Shader/material.h"
#include "../ImGUI/imgui_impl_glfw.h"
#include "../ImGUI/imgui_impl_opengl3.h"

/** UI */
#include "../UI/ContentBrowser/contentBrowser.h"
#include "../UI/EditorWindows/engineLogWindow.h"
#include "debugerTool.h"

const char* GEngineContentPath = "./Sources/EngineContent";
const char* GGameContentPath = "./Sources/Assets";

bool bIsGladLoaded = false;
bool bIsOpenglInitialized = false;
bool bIsImGuiInitialized = false;
double LastTickTime = 0;
GLFWwindow* openGLMainWindow = nullptr;

std::thread GGameThread;
std::thread GRenderThread;

void Engine::Initialize()
{
	StartOpenGL();

	InitializeImGui(openGLMainWindow);
	LoadEngineUILayout();

	LoadEngineAssets();
	LoadGameAssets();
	CreatePrimaryWorld();
	StartGameThread();
}

/************************************************************************/
/* OPENGL                                                               */
/************************************************************************/

void Engine::StartOpenGL()
{
	if (bIsOpenglInitialized) return;
	bIsOpenglInitialized = true;

	ProfileStat("");
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#if DISABLE_DOUBLE_BUFFERING
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
#endif
	GFullLog(LogVerbosity::Display, "Engine", "Initialize OpenGL GLFW (" + std::to_string(ReadCurrentStat()) + "ms)");

	CreateOpenGLMainWindow();
	InitializeGlad();
}
void Engine::CloseOpenGL()
{
	ProfileStat("");
	glfwTerminate();
	GFullLog(LogVerbosity::Display, "Engine", "Close OpenGL GLFW (" + std::to_string(ReadCurrentStat()) + "ms)");
}
void Engine::InitializeGlad()
{
	if (bIsGladLoaded) return;
	bIsGladLoaded = true;

	StartOpenGL();
	CreateOpenGLMainWindow();

	ProfileStat("");
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		GFullLog(LogVerbosity::Assert, "Engine", "Failed to create Glad");
	}
	GFullLog(LogVerbosity::Display, "Engine", "Initialize glad (" + std::to_string(ReadCurrentStat()) + "ms)");
}
void Engine::CreateOpenGLMainWindow()
{
	if (openGLMainWindow) return;

	StartOpenGL();

	ProfileStat("");
	openGLMainWindow = glfwCreateWindow(1600, 900, "GLEngine 1.0", NULL, NULL);
	if (openGLMainWindow == NULL)
	{
		GFullLog(LogVerbosity::Assert, "Engine", "Failed to create GLFW window");
	}
	glfwMakeContextCurrent(openGLMainWindow);
	GFullLog(LogVerbosity::Display, "Engine", "Create main window (" + std::to_string(ReadCurrentStat()) + "ms)");
}

/************************************************************************/
/* Get baseAssets / configs                                             */
/************************************************************************/

void Engine::LoadEngineAssets()
{
	ProfileStat("");
	AssetRegistry::ImportAssetFromDirectory(GEngineContentPath);
	GFullLog(LogVerbosity::Display, "Engine", "Import engine content (" + std::to_string(ReadCurrentStat()) + "ms)");
}
void Engine::LoadGameAssets()
{
	ProfileStat("");
	AssetRegistry::ImportAssetFromDirectory(GGameContentPath);
	GFullLog(LogVerbosity::Display, "Engine", "Import game content (" + std::to_string(ReadCurrentStat()) + "ms)");
}

/************************************************************************/
/* ImGui                                                                */
/************************************************************************/

void Engine::InitializeImGui(GLFWwindow* parentWindow)
{
	if (bIsImGuiInitialized) return;

	ProfileStat("");
	bIsImGuiInitialized = true;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplGlfw_InitForOpenGL(parentWindow, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	GFullLog(LogVerbosity::Display, "Engine", "Initialize ImGui (" + std::to_string(ReadCurrentStat()) + "ms)");
}
void Engine::CloseImGui()
{
	ProfileStat("");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	GFullLog(LogVerbosity::Display, "Engine", "Close ImGui (" + std::to_string(ReadCurrentStat()) + "ms)");
}
void Engine::LoadEngineUILayout()
{
	new ContentBrowser("Content browser");
	new EngineLogWindow("Console");
	new StatWindow("Debuger");
}

/************************************************************************/
/* Create world                                                         */
/************************************************************************/

void Engine::CreatePrimaryWorld()
{
	CreateOpenGLMainWindow();
	ProfileStat("");
	new World(openGLMainWindow);
	Material::InitializeMaterials();
	GFullLog(LogVerbosity::Display, "Engine", "Create primary world (" + std::to_string(ReadCurrentStat()) + "ms)");
}

/************************************************************************/
/* Tick management                                                      */
/************************************************************************/

void Engine::AsyncLoop()
{
	double DeltaSecond;
	while (!glfwWindowShouldClose(openGLMainWindow))
	{
		if (glfwGetTime() - LastTickTime < 1.0 / EditorWindow::GetMaxFramerate()) continue;
		DeltaSecond = glfwGetTime() - LastTickTime;
		if (DeltaSecond > 1.0 / 10.0)
		{
			DeltaSecond = 1.0 / 10.f;
		}
		LastTickTime = glfwGetTime();
		World::UpdateWorlds(DeltaSecond);
		StatViewer::FlushStats();
		StatViewer::FlushDrawcallsCount();

#if DISABLE_DOUBLE_BUFFERING
		glFlush();
#else
		glfwSwapBuffers(openGLMainWindow);
#endif
		glfwPollEvents();
	}
}

void Engine::StartGameThread()
{
	GFullLog(LogVerbosity::Display, "Engine", "Start tick loop");

	AsyncLoop();

	CloseImGui();
	CloseOpenGL();
}