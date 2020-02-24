#include <Windows.h>
#include <vector>
#include <Asset/asset.h>
#include <Asset/assetLibrary.h>
#include <Asset/AssetRegistry.h>

std::vector<Asset*> AssetRegistry;


std::vector<std::string> AssetLibrary::GetFilesInPath(std::string path)
{
	std::vector<std::string> names = {};
	std::string search_path = path + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

std::vector<std::string> AssetLibrary::GetFolderInPath(std::string path, bool returnBack /*= false*/)
{
	std::vector<std::string> names = {};
	std::string search_path = path + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				if (std::string(fd.cFileName) != std::string("."))
				{
					if (!returnBack)
					{
						if (std::string(fd.cFileName) != std::string(".."))
						{
							names.push_back(fd.cFileName);
						}
					}
					else
					{
						names.push_back(fd.cFileName);
					}
				}
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

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

std::string AssetLibrary::GetExtension(const std::string& filePath)
{
	bool bFoundDot = false;
	std::string outExtension = "";
	for (int i = filePath.size() - 1; i >= 0 && !bFoundDot; --i)
	{
		if (filePath.at(i) == '.')
		{
			bFoundDot = true;
		}
		else
		{
			outExtension = filePath[i] + outExtension;
		}
	}
	return bFoundDot ? outExtension : "";
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

std::string AssetLibrary::RemoveExtension(const std::string& filePath)
{
	std::string outString;
	int i;
	for (i = filePath.size() - 1; i >= 0 && filePath[i] != '.'; --i) { }
	for (int j = 0; j < i; ++j)
	{
		outString += filePath[j];
	}
	return outString;
}

std::string AssetLibrary::GenerateNonExistingAssetName(std::string baseName)
{
	if (AssetRegistry::FindAssetByName<Asset>(baseName))
	{
		int assetIndex = -1;
		do
		{
			assetIndex++;
		} while (AssetRegistry::FindAssetByName<Asset>(baseName + std::to_string(assetIndex)));
		return baseName + std::to_string(assetIndex);
	}
	else
	{
		return baseName;
	}
}