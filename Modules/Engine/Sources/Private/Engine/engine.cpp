#include <glad.h>
#include <glfw3/glfw3.h>

#include <Engine/engine.h>
#include "EngineLog/engineLog.h"
#include <Engine/debugerTool.h>
#include <imgui.h>
#include <UI/engineLogWindow.h>
#include <Shader/material.h>
#include <World/world.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <Engine/ThreadHandler.h>

bool bIsGladLoaded = false;
bool bIsOpenglInitialized = false;
bool bIsImGuiInitialized = false;
double LastTickTime = 0;
GLFWwindow* openGLMainWindow = nullptr;

std::thread::id GameThreadID;
std::thread GRenderThread;

Engine* GEngine = nullptr;

void Engine::Initialize()
{

	ProfileStat("EngineStartup");

	LoadAssets();

	GFullLog(LogVerbosity::Display, "Engine", "Complete engine startup (" + std::to_string(ReadCurrentStat()) + "ms)");
	StartGameThread();
}

Engine* Engine::Get()
{
	if (GEngine) return GEngine;
	GFullLog(LogVerbosity::Assert, "Engine", "Engine doesn't exist in current context");
	return nullptr;
}

void Engine::Start()
{
	Engine::Get()->Initialize();
}

void Engine::ShutDown()
{
	Engine::Get()->bRequestClose = true;
}

/************************************************************************/
/* OPENGL                                                               */
/************************************************************************/

void Engine::StartOpenGL()
{
	if (bIsOpenglInitialized) return;
	bIsOpenglInitialized = true;


	ProfileStat("EngineStartup");
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
	World::ClearWorlds();


	ProfileStat("EngineShutdown");
	glfwTerminate();
	GFullLog(LogVerbosity::Display, "Engine", "Close OpenGL GLFW (" + std::to_string(ReadCurrentStat()) + "ms)");
}
void Engine::InitializeGlad()
{
	if (bIsGladLoaded) return;
	bIsGladLoaded = true;

	StartOpenGL();
	CreateOpenGLMainWindow();


	ProfileStat("EngineStartup");
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


	ProfileStat("EngineStartup");
	openGLMainWindow = glfwCreateWindow(1600, 900, "GLEngine 1.0", NULL, NULL);
	if (openGLMainWindow == NULL)
	{
		GFullLog(LogVerbosity::Assert, "Engine", "Failed to create GLFW window");
	}
	glfwMakeContextCurrent(openGLMainWindow);
	GFullLog(LogVerbosity::Display, "Engine", "Create main window (" + std::to_string(ReadCurrentStat()) + "ms)");
}

/************************************************************************/
/* ImGui                                                                */
/************************************************************************/

void Engine::InitializeImGui(GLFWwindow* parentWindow)
{
	if (bIsImGuiInitialized) return;


	ProfileStat("EngineStartup");
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
	ProfileStat("EngineShutdown");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	GFullLog(LogVerbosity::Display, "Engine", "Close ImGui (" + std::to_string(ReadCurrentStat()) + "ms)");
}

/************************************************************************/
/* Create world                                                         */
/************************************************************************/

void Engine::CreatePrimaryWorld()
{
	ProfileStat("EngineStartup");
	CreateOpenGLMainWindow();
	CreateWorld(openGLMainWindow);
	Material::InitializeMaterials();
	GFullLog(LogVerbosity::Display, "Engine", "Create primary world (" + std::to_string(ReadCurrentStat()) + "ms)");
}

/************************************************************************/
/* Tick management                                                      */
/************************************************************************/

void Engine::AsyncLoop()
{
	GameThreadID = std::this_thread::get_id();
	double DeltaSecond;
	while (!bRequestClose)
	{
		Asset::FlushAsyncLoadingAssets();
		if (glfwGetTime() - LastTickTime < 1.0 / 60.f) continue;
		DeltaSecond = glfwGetTime() - LastTickTime;
		if (DeltaSecond > 1.0 / 10.0)
		{
			DeltaSecond = 1.0 / 10.f;
		}
		LastTickTime = glfwGetTime();
		World::TickWorlds(DeltaSecond);
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

	new ThreadHandler();
	ThreadHandler::Get()->StartGameThread();
	ThreadHandler::Get()->StartRenderThread();

	while (!DoesWantToShutDown()) {}

	delete ThreadHandler::Get();

}

std::thread::id Engine::GetGameThreadID()
{
	return GameThreadID;
}

bool Engine::DoesWantToShutDown()
{
	return Engine::Get()->bRequestClose;
}

void Engine::LoadOpenGL_RT()
{
	StartOpenGL();

	InitializeImGui(openGLMainWindow);
	CreateUI();

	CreatePrimaryWorld();
}

Engine::Engine()
{
	if (GEngine)
	{
		GFullLog(LogVerbosity::Assert, "Engine", "An other instance of engine is already running");
	}
	GEngine = this;
}

Engine::~Engine()
{
	GEngine = nullptr;
}