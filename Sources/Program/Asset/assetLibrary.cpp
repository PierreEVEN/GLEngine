#include "assetLibrary.h"
#include <Windows.h>
#include "asset.h"
#include "../Texture/texture.h"
#include "../Shader/material.h"
#include "../Mesh/staticMesh.h"
#include "../World/worldAsset.h"


std::vector<Asset*> AssetRegistry;


std::vector<std::string> AssetLibrary::CollectFilesUnderFolder(std::string folder)
{
	std::vector<std::string> names = {};
	std::string search_path = folder + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(folder + "/" + fd.cFileName);
			}
			else
			{
				if (std::string(fd.cFileName) != std::string("..") && std::string(fd.cFileName) != std::string(".") && fd.cFileName != folder)
				{
					for (auto& subFolder : CollectFilesUnderFolder(folder + "/" + fd.cFileName))
					{
						names.push_back(subFolder);
					}
				}
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

void AssetLibrary::LoadLibraryFiles(std::string RootFolder)
{
	std::vector<std::string> filesToLoad = CollectFilesUnderFolder(RootFolder);

	for (auto& filePath : filesToLoad)
	{
		if (CheckExtension(filePath, ".TextAsset"))
		{
			AssetRegistry.push_back(LoadAsset<Texture2D>(filePath));
		}
	}
	for (auto& filePath : filesToLoad)
	{
		if (CheckExtension(filePath, ".MatAsset"))
		{
			AssetRegistry.push_back(LoadAsset<Material>(filePath));
		}
	}
	for (auto& filePath : filesToLoad)
	{
		if (CheckExtension(filePath, ".SMAsset"))
		{
			AssetRegistry.push_back(LoadAsset<StaticMesh>(filePath));
		}
	}
	for (auto& filePath : filesToLoad)
	{
		if (CheckExtension(filePath, ".WorldAsset"))
		{
			AssetRegistry.push_back(LoadAsset<WorldAsset>(filePath));
		}
	}
}

bool AssetLibrary::CheckExtension(const std::string& filePath, const std::string& extension)
{
	int extensionLocation = extension.size() - 1;
	for (int i = filePath.size() - 1; i >= 0; --i)
	{
		if (extensionLocation < 0) return true;
		if (filePath[i] != extension[extensionLocation]) { return false; }

		extensionLocation--;
	}
	return false;
}

std::vector<Asset*> AssetLibrary::GetAssetRegistry() { return AssetRegistry; }
