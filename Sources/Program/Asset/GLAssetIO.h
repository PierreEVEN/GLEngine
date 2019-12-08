#ifndef GLASSETIO_H
#define GLASSETIO_H

#include <fstream>
#include <string>
#include <iostream>

enum SAssetPropertyType
{
	PropertyTypeNone = 0,
	PropertyTypeString = 1,
	PropertyTypeInt = 2,
	PropertyTypeBool = 3,
	PropertyTypeFloat = 4,
};

struct SPropertyValue
{
private:
	std::string propertyName;
	unsigned int bufferSize;
protected:
	SAssetPropertyType propertyType = PropertyTypeNone;
public:
	char* propertyValue;
	SPropertyValue(std::ifstream* fileStream, const std::string inPropertyName);
	bool IsValid() const { return propertyValue; }
	template<class T> void SetValue(T* value, unsigned int inBufferSize) { propertyValue = (char*)value; }
	template<class T> T* GetValue() const { return (T*)propertyValue; }
	std::string GetPropertyName() const { return propertyName; }
	unsigned int GetBufferSize() const { return bufferSize; }
	SAssetPropertyType GetPropertyType() const { return propertyType; }
};

struct SBoolPropertyValue : public SPropertyValue
{
	SBoolPropertyValue(std::ifstream* fileStream, const std::string inPropertyName) 
		: SPropertyValue(fileStream, inPropertyName) 
	{
		propertyType = PropertyTypeBool;
	}
	void SetBoolValue(bool inBoolValue) { SetValue<bool>(&inBoolValue, sizeof(bool)); }
	bool GetBoolValue() const { return *GetValue<bool>(); }
};

struct SIntPropertyValue : public SPropertyValue
{
	SIntPropertyValue(std::ifstream* fileStream, const std::string inPropertyName)
		: SPropertyValue(fileStream, inPropertyName)
	{
		propertyType = PropertyTypeInt;
	}
	void SetIntValue(int inIntValue) { SetValue<int>(&inIntValue, sizeof(int)); }
	int GetIntValue() const { return *GetValue<int>(); }
};

struct SFloatPropertyValue : public SPropertyValue
{
	SFloatPropertyValue(std::ifstream* fileStream, const std::string inPropertyName)
		: SPropertyValue(fileStream, inPropertyName)
	{
		propertyType = PropertyTypeFloat;
	}
	void SetFloatValue(float inFloatValue) { SetValue<float>(&inFloatValue, sizeof(float)); }
	float GetFloatValue() const { return *GetValue<float>(); }
};

struct SStringPropertyValue : public SPropertyValue
{
	SStringPropertyValue(std::ifstream* fileStream, const std::string inPropertyName)
		: SPropertyValue(fileStream, inPropertyName)
	{
		propertyType = PropertyTypeString;
	}
	void SetStringValue(std::string inStringValue)
	{
		memcpy(propertyValue, inStringValue.data(), inStringValue.size() + 1);
	}
	std::string GetStringValue() const {
		return GetValue<const char>(); 
	}
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

	void ForceCloseFile();
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