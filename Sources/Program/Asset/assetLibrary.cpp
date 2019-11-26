#include "assetLibrary.h"
#include <Windows.h>
#include "asset.h"
#include "../Texture/texture.h"
#include "../Shader/material.h"
#include "../Mesh/staticMesh.h"


std::vector<Asset*> AssetRegistry;


std::vector<std::string> AssetLibrary::CollectFilesUnderFolder(std::string folder)
{
	std::vector<std::string> names;
	std::string search_path = folder + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..

			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(folder + "/" + fd.cFileName);
			}
			else
			{
				if (std::string(fd.cFileName) != std::string("..") && std::string(fd.cFileName) != std::string(".") && fd.cFileName != folder)
				{
					for (auto& subFolder : CollectFilesUnderFolder(folder + "/" + fd.cFileName))
					{
						names.push_back(folder + "/" + subFolder);
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

	Asset* newAsset = nullptr;

	for (auto& filePath : filesToLoad)
	{
		if (CheckExtension(filePath, ".TextAsset"))
		{
			newAsset = new Texture2D(filePath);
		}
	}

	for (auto& filePath : filesToLoad)
	{
		if (CheckExtension(filePath, ".MatAsset"))
		{
			newAsset = new Material(filePath);
		}
	}
	for (auto& filePath : filesToLoad)
	{
		if (CheckExtension(filePath, ".SMAsset"))
		{
			newAsset = new StaticMesh(filePath);
		}
	}
	for (auto& filePath : filesToLoad)
	{
		if (CheckExtension(filePath, ".World"))
		{

		}
	}
	if (newAsset)
	{
		newAsset->LoadAsset();
		AssetRegistry.push_back(newAsset);
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

Asset* AssetLibrary::FindAssetByName(std::string AssetName)
{
	for (auto& asset : AssetRegistry)
	{
		if (asset->GetName() == AssetName)
		{
			return asset;
		}
	}
	return nullptr;
}