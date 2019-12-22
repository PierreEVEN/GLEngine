#pragma once

#include <fstream>
#include <string>
#include <iostream>

class Asset;

enum SAssetPropertyType
{
	PropertyTypeNone = 0,
	PropertyTypeString = 1,
	PropertyTypeUInt = 2,
	PropertyTypeInt = 3,
	PropertyTypeBool = 4,
	PropertyTypeFloat = 5,
	PropertyTypeAssetRef = 6,
	PropertyTypeFileRef = 7,
};

struct SPropertyValue
{
private:
	std::string propertyName;
	unsigned int bufferSize;
	bool bIsDirty;
	Asset* owner;
protected:
	SAssetPropertyType propertyType = PropertyTypeNone;
public:
	char* propertyValue;
	SPropertyValue(std::string inPropertyName);
	SPropertyValue(Asset* inParentAsset, std::ifstream* fileStream, const std::string inPropertyName);
	bool IsValid() const { return propertyValue; }
	template<class T> void SetValue(T* value, unsigned int inBufferSize) {
		propertyValue = (char*)value;
		MarkPropertyDirty();
	}
	template<class T> T* GetValue() const { return (T*)propertyValue; }
	std::string GetPropertyName() const { return propertyName; }
	unsigned int GetBufferSize() const { return bufferSize; }
	Asset* GetOwner() const { return owner; }
	void MarkPropertyDirty();
	bool IsDirty() const { return bIsDirty; }
	SAssetPropertyType GetPropertyType() const { return propertyType; }
};

struct SBoolPropertyValue : public SPropertyValue
{
	SBoolPropertyValue(Asset* inParentAsset, std::ifstream* fileStream, const std::string inPropertyName)
		: SPropertyValue(inParentAsset, fileStream, inPropertyName)
	{
		propertyType = PropertyTypeBool;
	}
	void SetBoolValue(bool inBoolValue) { SetValue<bool>(&inBoolValue, sizeof(bool)); }
	bool GetBoolValue() const { return *GetValue<bool>(); }
};

struct SUIntPropertyValue : public SPropertyValue
{
	SUIntPropertyValue(Asset* inParentAsset, std::ifstream* fileStream, const std::string inPropertyName)
		: SPropertyValue(inParentAsset, fileStream, inPropertyName)
	{
		propertyType = PropertyTypeUInt;
	}
	void SetUIntValue(unsigned int inIntValue) { SetValue<unsigned int>(&inIntValue, sizeof(unsigned int)); }
	unsigned int GetUIntValue() const { return *GetValue<unsigned int>(); }
};

struct SIntPropertyValue : public SPropertyValue
{
	SIntPropertyValue(Asset* inParentAsset, std::ifstream* fileStream, const std::string inPropertyName)
		: SPropertyValue(inParentAsset, fileStream, inPropertyName)
	{
		propertyType = PropertyTypeInt;
	}
	void SetIntValue(int inIntValue) { SetValue<int>(&inIntValue, sizeof(int)); }
	int GetIntValue() const { return *GetValue<int>(); }
};

struct SFloatPropertyValue : public SPropertyValue
{
	SFloatPropertyValue(Asset* inParentAsset, std::ifstream* fileStream, const std::string inPropertyName)
		: SPropertyValue(inParentAsset, fileStream, inPropertyName)
	{
		propertyType = PropertyTypeFloat;
	}
	void SetFloatValue(float inFloatValue) { SetValue<float>(&inFloatValue, sizeof(float)); }
	float GetFloatValue() const { return *GetValue<float>(); }
};

struct SStringPropertyValue : public SPropertyValue
{
	SStringPropertyValue(std::string inPropertyName) : SPropertyValue(inPropertyName) {}
	SStringPropertyValue(Asset* inParentAsset, std::ifstream* fileStream, const std::string inPropertyName)
		: SPropertyValue(inParentAsset, fileStream, inPropertyName)
	{
		propertyType = PropertyTypeString;
	}
	void SetStringValue(std::string inStringValue)
	{
		memcpy(propertyValue, inStringValue.data(), inStringValue.size() + 1);
		MarkPropertyDirty();
	}
	std::string GetStringValue() const {
		return GetValue<const char>(); 
	}
};

struct SAssetRefPropertyValue : public SPropertyValue
{
	SAssetRefPropertyValue(Asset* inParentAsset, std::ifstream* fileStream, const std::string inPropertyName)
		: SPropertyValue(inParentAsset, fileStream, inPropertyName)
	{
		propertyType = PropertyTypeAssetRef;
	}
	void SetAssetRef(std::string inStringValue)
	{
		memcpy(propertyValue, inStringValue.data(), inStringValue.size() + 1);
		MarkPropertyDirty();
	}
	std::string GetAssetRef() const {
		return GetValue<const char>();
	}
};

struct SFileRefPropertyValue : public SPropertyValue
{
	SFileRefPropertyValue(Asset* inParentAsset, std::ifstream* fileStream, const std::string inPropertyName)
		: SPropertyValue(inParentAsset, fileStream, inPropertyName)
	{
		propertyType = PropertyTypeFileRef;
	}
	void SetFileRef(std::string inStringValue)
	{
		memcpy(propertyValue, inStringValue.data(), inStringValue.size() + 1);
		MarkPropertyDirty();
	}
	std::string GetFileRef() const {
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
	SAssetWriter(std::string inAssetPath, bool bResetFile = false);
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
	static void SaveAssetProperties(Asset* inAsset);
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