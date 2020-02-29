#include "Engine/ThreadHandler.h"
#include <EngineLog/engineLog.h>
#include <Engine/engine.h>
#include <World/world.h>
#include <Asset/asset.h>

ThreadHandler* ThreadHandlerReference = nullptr;
double LastGameThreadTickTime = 0.0;
double LastRenderThreadTickTime = 0.0;
double GameThreadDeltaSecond = 0.0;
double RenderThreadDeltaSecond = 0.0;

ThreadHandler::ThreadHandler()
{
	ThreadHandlerReference = this;
}

ThreadHandler::~ThreadHandler()
{
	CloseThreads();
	ThreadHandlerReference = nullptr;
}

bool ThreadHandler::IsGameThread()
{
	if (!ThreadHandler::Get()->GameThreadRef) GFullLog(LogVerbosity::Assert, typeid(ThreadHandler).name(), "Cannot find GameThread");
	return std::this_thread::get_id() == ThreadHandler::Get()->GameThreadRef->get_id();
}

bool ThreadHandler::IsRenderThread()
{
	if (!ThreadHandler::Get()->RenderThreadRef) GFullLog(LogVerbosity::Assert, typeid(ThreadHandler).name(), "Cannot find RenderThread");
	return std::this_thread::get_id() == ThreadHandler::Get()->RenderThreadRef->get_id();
}

ThreadHandler* ThreadHandler::Get()
{
	if (!ThreadHandlerReference) GFullLog(LogVerbosity::Assert, typeid(ThreadHandler).name(), "ThreadHandler doesn't exist in current context");
	return ThreadHandlerReference;
}

void ThreadHandler::StartGameThread()
{
	if (GameThreadRef) GLogAssert("Game Thread already exist");
	GameThreadRef = new std::thread(GameThread);
}

void ThreadHandler::StartRenderThread()
{
	if (RenderThreadRef) GLogAssert("Render Thread already exist");
	RenderThreadRef = new std::thread(RenderThread);
}

double ThreadHandler::GetGameThreadDeltaTime()
{
	return GameThreadDeltaSecond;
}

double ThreadHandler::GetRenderThreadDeltaTime()
{
	return RenderThreadDeltaSecond;
}

void ThreadHandler::CloseThreads() {
	if (RenderThreadRef && RenderThreadRef->joinable()) {
		RenderThreadRef->join();
		delete RenderThreadRef;
	}
	if (GameThreadRef && GameThreadRef->joinable()) {
		GameThreadRef->join();
		delete GameThreadRef;
	}
}

void ThreadHandler::GameThread()
{
	while (!Engine::DoesWantToShutDown())
	{
		Asset::FlushAsyncLoadingAssets();
		//if (glfwGetTime() - LastGameThreadTickTime < 1.0 / 60.f) continue;
		GameThreadDeltaSecond = glfwGetTime() - LastGameThreadTickTime;
		LastGameThreadTickTime = glfwGetTime();

		World::TickWorlds(GameThreadDeltaSecond);
	}
}

void ThreadHandler::RenderThread()
{
	Engine::Get()->LoadOpenGL_RT();
	while (!Engine::DoesWantToShutDown())
	{
		
		RenderThreadDeltaSecond = glfwGetTime() - LastRenderThreadTickTime;
		LastRenderThreadTickTime = glfwGetTime();

		World::RenderWorlds();

#if DISABLE_DOUBLE_BUFFERING
		glFlush();
#else
		glfwSwapBuffers(openGLMainWindow);
#endif
		glfwPollEvents();
	}

	Engine::Get()->CloseImGui();
	Engine::Get()->CloseOpenGL();
}
