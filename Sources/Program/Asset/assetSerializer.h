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
	unsigned int bufferSize;
	char* propertyValue;
	SPropertyValue() {}
	SPropertyValue(std::ifstream* fileStream, const std::string inPropertyName)
		: propertyName(inPropertyName) 
	{
		unsigned int fieldCursorLocation;
		unsigned int fieldBufferSize;
		if (AssetSerializer::FindField(fileStream, propertyName, fieldCursorLocation, fieldBufferSize))
		{
			bufferSize = fieldBufferSize;
			AssetSerializer::ReadFieldValue<char>(fileStream, fieldCursorLocation, fieldBufferSize, propertyValue);
		}
	}



	template<class T> void SetValue(T* value)
	{
		SetValue<T>(value, sizeof(T));
	}

	template<class T> void SetValue(T* value, unsigned int inBufferSize)
	{
		propertyValue = (char*)value;
	}

	template<class T> T* GetValue() const
	{
		return (T*)propertyValue;
	}

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
		int arrayTest[] = { 1, 5, 56, 12, 28 };
		char testChar[7] = "tototo";
		int testInt = 16;


		/************************************************************************/
		/* Write sequence                                                       */
		/************************************************************************/
		std::cout << "Begin write sequence..." << std::endl;
		std::ofstream* oStream = BeginWrite("./testFile.txt");
		AppendField<int*>(oStream, "Property1", arrayTest, sizeof(arrayTest));
		AppendField<int*>(oStream, "Property2", &testInt, sizeof(int));
		EndWrite(oStream);
		if (!oStream->good()) {
			std::cout << "Error occurred at writing time!" << std::endl;
		}

		std::cout << "Begin read sequence..." << std::endl;
		std::ifstream* iStream = BeginRead("./testFile.txt");
		unsigned int readerPosition;
		unsigned int valueBufferSize;		

		/************************************************************************/
		/* Read sequence                                                        */
		/************************************************************************/
		std::string propertyName = "Property2";
		if (FindField(iStream, propertyName, readerPosition, valueBufferSize))
		{
			std::cout << "found property " << propertyName << " : readerPosition =" << readerPosition << " valueBufferSize=" << valueBufferSize << std::endl;
			int* testFoundValue = (int*) malloc(20);
			unsigned int bufferSizeCpy = valueBufferSize;
			ReadFieldValue<int>(iStream, readerPosition, valueBufferSize, testFoundValue);

			SPropertyValue testPropertyValue;
			testPropertyValue.SetValue<int>(testFoundValue);

			std::cout << *testPropertyValue.GetValue<int>() << std::endl;


//			std::cout << "RESULT : " << testFoundValue << std::endl;
			for (unsigned int i = 0; i < bufferSizeCpy / 4; ++i)
			{
				std::cout << "RESULT : " << testFoundValue[i] << std::endl;
			}
		}
		
		EndRead(iStream);
		if (!iStream->good()) {
			std::cout << "Error occurred at reading time!" << std::endl;
		}
	}

	static std::ifstream* BeginRead(std::string filePath);
	static void EndRead(std::ifstream* filePath);

	static std::ofstream* BeginWrite(std::string filePath);
	static void EndWrite(std::ofstream* filePath);

	template <class T> static void AppendField(std::ofstream* outputFileStream, std::string fieldName, T value, unsigned int bufferSize);
	static bool ReadField(std::ifstream* outputFileStream, std::string& propertyName, unsigned int& readerPosition, unsigned int& valueBufferSize);
	static bool FindField(std::ifstream* outputFileStream, const std::string propertyName, unsigned int& readerPosition, unsigned int& valueBufferSize);
	template <class T> static bool ReadFieldValue(std::ifstream* outputFileStream, const unsigned int& position, const unsigned int& bufferSize, T* result);
};

template <class T>
bool AssetSerializer::ReadFieldValue(std::ifstream* outputFileStream, const unsigned int& position, const unsigned int& bufferSize, T* result)
{
	outputFileStream->seekg(position);
	outputFileStream->read(reinterpret_cast<char*>(result), bufferSize);
	return true;
}

template <class T>
void AssetSerializer::AppendField(std::ofstream* outputFileStream, std::string fieldName, T value, unsigned int bufferSize)
{


	unsigned int propertyNameSize = fieldName.length() + 1;
	std::cout << "write property name buffer size : " << propertyNameSize << std::endl;
	outputFileStream->write(reinterpret_cast<char*>(&propertyNameSize), sizeof(unsigned int));



	const char* propertyNameValue = fieldName.c_str();
	std::cout << "Write property name : " << propertyNameValue << std::endl;
	for (unsigned int i = 0; i < propertyNameSize; ++i)
	{
		char val = propertyNameValue[i];
		outputFileStream->write(reinterpret_cast<char*>(&val), 1);
	}


	std::cout << "write property value buffer size : " << bufferSize << std::endl;
	outputFileStream->write(reinterpret_cast<char*>(&bufferSize), sizeof(unsigned int));
	outputFileStream->write(reinterpret_cast<char*>(value), bufferSize);

	for (unsigned int i = 0; i < bufferSize / 4; ++i)
	{
		//std::cout << "Writing value : " << value[i] << std::endl;
	}

}




#endif