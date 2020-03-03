#include <Editor/EditorEngine.h>
#include "World/EditorWorld.h"
#include <EngineLog/engineLog.h>
#include <Engine/debugerTool.h>
#include <Asset/AssetRegistry.h>
#include <UI/EditorWindows/contentBrowser.h>
#include <UI/engineLogWindow.h>

void EditorEngine::LoadAssets()
{
	AssetRegistry::ImportAssetFromDirectory("./Sources/EngineContent");
	AssetRegistry::ImportAssetFromDirectory("./Sources/Assets");
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
}

