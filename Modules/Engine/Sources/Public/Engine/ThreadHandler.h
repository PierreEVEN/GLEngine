
#pragma once
#include <thread>

#define CHECK_GAME_THREAD if (!ThreadHandler::Get()->IsGameThread()) GLogAssert("must be used on Game Thread");
#define CHECK_RENDER_THREAD if (!ThreadHandler::Get()->IsRenderThread()) GLogAssert("must be used on Render Thread");

class ThreadHandler
{
public:

	ThreadHandler();
	virtual ~ThreadHandler();

	static bool IsGameThread();

	static bool IsRenderThread();

	static ThreadHandler* Get();

	void StartGameThread();
	void StartRenderThread();

	std::thread* GetGameThread() const { return GameThreadRef; }
	std::thread* GetRenderThread() const { return RenderThreadRef; }

	static double GetGameThreadDeltaTime();
	static double GetRenderThreadDeltaTime();

	virtual void CloseThreads();
private:

	std::thread* RenderThreadRef = nullptr;
	std::thread* GameThreadRef = nullptr;

	static void GameThread();
	static void RenderThread();
};