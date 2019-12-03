#ifndef ASSETSERIALIZER_H
#define ASSETSERIALIZER_H

#include <fstream>
#include <iostream>
#include <assert.h>
#include <string>
#include <vector>
#include <iosfwd>

struct SPropertyValue
{
	std::string propertyName;
	unsigned int elements;
	const char* propertyValue;
	SPropertyValue() {}
	SPropertyValue(const std::string inPropertyName, const unsigned int inElements, const char* inPropertyValue)
		: propertyName(inPropertyName), elements(inElements), propertyValue(inPropertyValue) {}

	bool IsValid() const { return propertyName != "" && propertyValue != ""; }
};

struct SPropertyWrapper
{
	SPropertyWrapper() {}
	std::vector<SPropertyValue> properties;

	void AddProperty(SPropertyValue InProperty)	{ properties.push_back(InProperty);	}

};

class AssetSerializer
{
private:

	static bool IsField(const std::string fieldName, const std::string lineString);
	static bool GetFieldValue(const std::string fieldName, const std::string lineString, std::string& fieldValue);
	static bool ExtractFieldName(std::string& fieldName, const std::string lineString);

public:

	static bool GetFileTypeString(const std::string filePath, std::string& fileTypeString);

	static std::vector<std::string> GetFilePropertyNames(const std::string filePath);

	static std::vector<SPropertyValue> ReadFields(const std::string filePath, std::vector<std::string> fieldNames);
	static std::string ReadField(const std::string filePath, const std::string searchedField, bool bAssertIfFailed = false);

	static bool WriteField(const std::string filePath, const std::string fieldName, const std::string value);
	

	static void TestDebug()
	{
		std::ofstream* oStream = BeginWrite("./testFile.txt");
		AppendField<int>(oStream, "test45", 148, 10);
		AppendField<int>(oStream, "second", 123, 14);
		EndWrite(oStream);
		if (!oStream->good()) {
			std::cout << "Error occurred at writing time!" << std::endl;
		}

		std::ifstream* iStream = BeginRead("./testFile.txt");
		SPropertyValue testValue;
		ReadNextField<int>(testValue, iStream);
		ReadNextField<int>(testValue, iStream);
		//std::cout << "Property name value : " << testValue.propertyName << std::endl;
		while (true) {}
		EndRead(iStream);
		if (!iStream->good()) {
			std::cout << "Error occurred at reading time!" << std::endl;
		}

		//std::cout << testValue.propertyName << " | " << testValue.propertyValue << std::endl;
	}


	static std::ifstream* BeginRead(std::string filePath);
	static void EndRead(std::ifstream* filePath);

	static std::ofstream* BeginWrite(std::string filePath);
	static void EndWrite(std::ofstream* filePath);

	template <class T>	static void AppendField(std::ofstream* outputFileStream, std::string fieldName, T value, unsigned int elements = 1);
	template <class T>  static bool ReadNextField(SPropertyValue& value, std::ifstream* outputFileStream);
};

template <class T>
void AssetSerializer::AppendField(std::ofstream* outputFileStream, std::string fieldName, T value, unsigned int elements)
{
	unsigned int propertyNameSize = fieldName.length() + 1;
	std::cout << "in data size : " << propertyNameSize << std::endl;
	const char* propertyNameValue = fieldName.c_str();


	for (int i = 0; i < 7; ++i)
	{
		std::cout << "test : " << (int)propertyNameValue[i] << std::endl;

	}

	outputFileStream->write(reinterpret_cast<char*>(&propertyNameSize), sizeof(unsigned int));
	outputFileStream->write(reinterpret_cast<char*>(&propertyNameValue), propertyNameSize);
	outputFileStream->write(reinterpret_cast<char*>(&value), sizeof(T));
}

template <class T>
bool AssetSerializer::ReadNextField(SPropertyValue& value, std::ifstream* outputFileStream)
{	
	const char* buffer;
	unsigned int bufferSize;
	T foundValue;
	outputFileStream->read(reinterpret_cast<char*>(&bufferSize), sizeof(unsigned int));
	std::cout << "found buffer size : " << bufferSize << std::endl;
	outputFileStream->read(reinterpret_cast<char*>(&buffer), bufferSize);
	outputFileStream->read(reinterpret_cast<char*>(&foundValue), sizeof(T));


	for (int i = 0; i < 7; ++i)
	{
		std::cout << "test : " << (int)buffer[i] << std::endl;

	}

	std::cout << "Property name value : " << buffer << std::endl;
	std::cout << "Property value value : " << std::to_string(foundValue) << std::endl;
	return true;
}

#endif