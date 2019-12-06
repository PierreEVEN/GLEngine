#include "GLAssetIO.h"
#include <iostream>

SPropertyValue::SPropertyValue(std::ifstream* fileStream, const std::string inPropertyName)
	: propertyName(inPropertyName)
{
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

SAssetReader::SAssetReader(std::string inAssetPath)
{
	assetPath = inAssetPath;
	fileStream = new std::ifstream(assetPath, std::ios::in | std::ios::binary);
	if (!fileStream || !fileStream->is_open())
	{
		std::cout << "Failed to open file '" << assetPath << "'" << std::endl;
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
			std::cout << "Failed to close file '" << assetPath << "'" << std::endl;
		}
	}
}

SAssetWriter::SAssetWriter(std::string inAssetPath)
{
	assetPath = inAssetPath;
	fileStream = new std::ofstream(assetPath, std::ios::out | std::ios::binary);
	if (!fileStream || !fileStream->is_open()) {
		std::cout << "Failed to open file '" << assetPath << "'" << std::endl;
		delete fileStream;
		fileStream = nullptr;
	}
}

SAssetWriter::~SAssetWriter()
{
	if (fileStream)
	{
		fileStream->close();
		if (!fileStream->good()) {
			std::cout << "Failed to close file '" << assetPath << "'" << std::endl;
		}
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