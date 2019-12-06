#ifndef GLASSETIO_H
#define GLASSETIO_H

#include <fstream>
#include <string>

struct SPropertyValue
{
	std::string propertyName;
	unsigned int bufferSize;
	char* propertyValue;
	SPropertyValue(std::ifstream* fileStream, const std::string inPropertyName);

	bool IsValid() const { return propertyValue; }

	template<class T> void SetValue(T* value) { SetValue<T>(value, sizeof(T)); }
	template<class T> void SetValue(T* value, unsigned int inBufferSize) { propertyValue = (char*)value; }
	template<class T> T* GetValue() const { return (T*)propertyValue; }
};

struct SAssetReader
{
private:
	std::string assetPath;
	std::ifstream* fileStream;
public:
	bool IsValid() const { return fileStream; }
	std::ifstream* Get() { return fileStream; }
	SAssetReader(std::string inAssetPath);
	~SAssetReader();
};

struct SAssetWriter
{
private:
	std::string assetPath;
	std::ofstream* fileStream;
public:
	bool IsValid() const { return fileStream; }
	std::ofstream* Get() { return fileStream; }
	SAssetWriter(std::string inAssetPath);
	~SAssetWriter();
};

class GLAssetIO
{
private:

	static bool ReadField(std::ifstream* outputFileStream, std::string& propertyName, unsigned int& readerPosition, unsigned int& valueBufferSize, const std::streamoff& fileSize);

public:

	template <class T> static void AppendField(std::ofstream* outputFileStream, std::string fieldName, T value, unsigned int bufferSize);
	static bool FindField(std::ifstream* outputFileStream, const std::string propertyName, unsigned int& readerPosition, unsigned int& valueBufferSize);
	template <class T> static bool ReadFieldValue(std::ifstream* outputFileStream, const unsigned int& position, const unsigned int& bufferSize, T* result);
	static void GenerateFileBody(std::ofstream* newFileStream, std::string newAssetName, std::string assetType);
};

template <class T>
bool GLAssetIO::ReadFieldValue(std::ifstream* outputFileStream, const unsigned int& position, const unsigned int& bufferSize, T* result)
{
	outputFileStream->seekg(position);
	outputFileStream->read(reinterpret_cast<char*>(result), bufferSize);
	return true;
}

template <class T>
void GLAssetIO::AppendField(std::ofstream* outputFileStream, std::string fieldName, T value, unsigned int bufferSize)
{
	unsigned int propertyNameSize = fieldName.length() + 1;
	outputFileStream->write(reinterpret_cast<char*>(&propertyNameSize), sizeof(unsigned int));
	   
	const char* propertyNameValue = fieldName.c_str();

	for (unsigned int i = 0; i < propertyNameSize; ++i)
	{
		char val = propertyNameValue[i];
		outputFileStream->write(reinterpret_cast<char*>(&val), 1);
	}

	outputFileStream->write(reinterpret_cast<char*>(&bufferSize), sizeof(unsigned int));
	outputFileStream->write(reinterpret_cast<char*>(value), bufferSize);
}
#endif