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
		AppendField<std::string>(oStream, "testField", std::string("XValueX"));
		EndWrite(oStream);

		std::ifstream* iStream = BeginRead("./testFile.txt");
		SPropertyValue testValue;
		ReadNextField<std::string>(testValue, iStream);

		std::cout << testValue.propertyName << " | " << testValue.propertyValue << std::endl;

		EndRead(iStream);
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
	fieldName += "=";
	outputFileStream->write((char*)fieldName.data(), sizeof(fieldName));

	outputFileStream->write((char*)&elements, sizeof(unsigned int));

	for (unsigned int i = 0; i < elements; ++i)
	{
		outputFileStream->write((char*)&value[0], sizeof(std::string("XValueX")));
	}
}

template <class T>
bool AssetSerializer::ReadNextField(SPropertyValue& value, std::ifstream* outputFileStream)
{
	value.propertyName = "";
	value.propertyValue;
	const char* nextChar;
	while (outputFileStream->read((char*)&nextChar, sizeof(const char*)) && nextChar != (const char*)'=')
	{
		std::cout << nextChar << std::endl;
		value.propertyName += nextChar;
	}
	std::cout << "PropertyName : " << value.propertyName << std::endl;

	outputFileStream->read((char*)&value.elements, sizeof(unsigned int));

	std::cout << "Elements : " << std::to_string(value.elements) << std::endl;
	value.propertyValue = nullptr;


	std::vector<T> values(value.elements);

	for (unsigned int i = 0; i < value.elements; ++i)
	{
		outputFileStream->read((char*)&values[i], sizeof(T));
	}
	return true;
}

#endif