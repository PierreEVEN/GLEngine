#include <Editor/EditorEngine.h>
#include "World/EditorWorld.h"
#include <EngineLog/engineLog.h>
#include <Engine/debugerTool.h>
#include <Asset/AssetRegistry.h>
#include <UI/EditorWindows/contentBrowser.h>
#include <UI/engineLogWindow.h>

void EditorEngine::LoadAssets()
{
	{
		ProfileStat("");
		AssetRegistry::ImportAssetFromDirectory("./Sources/EngineContent");
		GFullLog(LogVerbosity::Display, "Engine", "Import engine content (" + std::to_string(ReadCurrentStat()) + "ms)");
	}
	{
		ProfileStat("");
		AssetRegistry::ImportAssetFromDirectory("./Sources/Assets");
		GFullLog(LogVerbosity::Display, "Engine", "Import game content (" + std::to_string(ReadCurrentStat()) + "ms)");
	}
}

void EditorEngine::CreateUI()
{
	new ContentBrowser("Content browser");
	new EngineLogWindow("Console");
	new StatWindow("Debuger");
}

void EditorEngine::CreateWorld(GLFWwindow* inWindow)
{
	EditorWorld* createdWorld = new EditorWorld(inWindow);
	createdWorld->Initialize();
}

