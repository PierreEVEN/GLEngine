#include "GLAssetIO.h"
#include <iostream>
#include "../EngineLog/engineLog.h"
#include "asset.h"

SPropertyValue::SPropertyValue(Asset* inParentAsset, std::ifstream* fileStream, const std::string inPropertyName)
	: SPropertyValue(inPropertyName)
{
	bIsDirty = false;
	owner = inParentAsset;
	unsigned int fieldCursorLocation;
	unsigned int fieldBufferSize;
	if (GLAssetIO::FindField(fileStream, propertyName, fieldCursorLocation, fieldBufferSize))
	{
		propertyValue = (char*)malloc(fieldBufferSize);
		bufferSize = fieldBufferSize;
		GLAssetIO::ReadFieldValue<char>(fileStream, fieldCursorLocation, fieldBufferSize, propertyValue);
	}
	else
	{
		propertyValue = nullptr;
	}
}

SPropertyValue::SPropertyValue(std::string inPropertyName)
{
	propertyName = inPropertyName;
	bIsDirty = false;
	propertyValue = nullptr;
	bufferSize = 0;
	owner = nullptr;
}

void SPropertyValue::MarkPropertyDirty()
{
	bIsDirty = true;
	if (GetOwner())
	{
		GetOwner()->MarkAssetDirty();
	}
}

SAssetReader::SAssetReader(std::string inAssetPath)
{
	assetPath = inAssetPath;
	fileStream = new std::ifstream(assetPath, std::ios::in | std::ios::binary | std::ios::binary);
	if (!fileStream || !fileStream->is_open())
	{
		GLog(LogVerbosity::Error, "AssetLoading", "Failed to open file '" + assetPath + "' : " + (fileStream ? " invalid path " : " cannot open file "));
		delete fileStream;
		fileStream = nullptr;
	}
}

SAssetReader::~SAssetReader()
{
	if (fileStream)
	{
		fileStream->close();
		if (!fileStream->good()) {
			GLog(LogVerbosity::Error, "AssetLoading", "Failed to close file '" + assetPath + "'");
		}
	}
}

SAssetWriter::SAssetWriter(std::string inAssetPath, bool bResetFile)
{
	assetPath = inAssetPath;
	if (bResetFile)
	{
		fileStream = new std::ofstream(assetPath, std::ios::out | std::ios::binary | std::ios::trunc);
	}
	else
	{
		fileStream = new std::ofstream(assetPath, std::ios::out | std::ios::binary);
	}
	if (!fileStream || !fileStream->is_open()) {
		GLog(LogVerbosity::Error, "AssetLoading", "Failed to open file '" + assetPath + "' : " + (fileStream ? " invalid path " : " cannot open file "));
		delete fileStream;
		fileStream = nullptr;
	}
}

SAssetWriter::~SAssetWriter()
{
	ForceCloseFile();
}

void SAssetWriter::ForceCloseFile()
{
	if (fileStream)
	{
		fileStream->close();
		if (!fileStream->good()) {
			GLog(LogVerbosity::Error, "AssetLoading", "Failed to close file '" + assetPath + "'");
		}
		fileStream = nullptr;
	}
}

bool GLAssetIO::ReadField(std::ifstream* outputFileStream, std::string& propertyName, unsigned int& valueReaderPosition, unsigned int& valueBufferSize, const std::streamoff& fileSize)
{
	if ((unsigned int)outputFileStream->tellg() + sizeof(unsigned int) >= fileSize) return false;
	unsigned int nameBufferSize;
	outputFileStream->read(reinterpret_cast<char*>(&nameBufferSize), sizeof(unsigned int));

	if ((unsigned int)outputFileStream->tellg() + nameBufferSize >= fileSize) return false;
	char* valTest = (char*)malloc(nameBufferSize);
	outputFileStream->read(reinterpret_cast<char*>(valTest), nameBufferSize);
	propertyName = std::string(valTest);

	if ((unsigned int)outputFileStream->tellg() + sizeof(unsigned int) >= fileSize) return false;
	outputFileStream->read(reinterpret_cast<char*>(&valueBufferSize), sizeof(unsigned int));
	valueReaderPosition = (int)outputFileStream->tellg();
	outputFileStream->seekg((int)outputFileStream->tellg() + valueBufferSize);

	return true;
}
bool GLAssetIO::FindField(std::ifstream* outputFileStream, const std::string propertyName, unsigned int& readerPosition, unsigned int& valueBufferSize)
{
	outputFileStream->seekg(0, std::ios::end);
	std::streamoff fileSize = outputFileStream->tellg();
	outputFileStream->seekg(0, std::ios::beg);
	std::string currentPropertyName;
	while (ReadField(outputFileStream, currentPropertyName, readerPosition, valueBufferSize, fileSize) && propertyName != currentPropertyName) { }
	if (propertyName != currentPropertyName) return false;
	return true;
}

void GLAssetIO::GenerateFileBody(std::ofstream* newFileStream, std::string newAssetName, std::string assetType)
{
	GLAssetIO::AppendField<char*>(newFileStream, "AssetType", (char*)(assetType.data()), assetType.size() + 1);
	GLAssetIO::AppendField<char*>(newFileStream, "AssetName", (char*)(newAssetName.data()), newAssetName.size() + 1);
}

void GLAssetIO::SaveAssetProperties(Asset* inAsset)
{
	if (!inAsset || !inAsset->HasValidPath()) return;
	SAssetWriter writer(inAsset->GetPath(), true);
	GLAssetIO::GenerateFileBody(writer.Get(), inAsset->GetName(), inAsset->GetAssetType());
	for (const auto& prop : inAsset->GetAssetProperties())
	{
		if (prop && prop->GetPropertyName() != "AssetType" && prop->GetPropertyName() != "AssetName")
		{
			GLAssetIO::AppendField<char*>(writer.Get(), prop->GetPropertyName(), prop->GetValue<char>(), prop->GetBufferSize());
		}
	}
}

