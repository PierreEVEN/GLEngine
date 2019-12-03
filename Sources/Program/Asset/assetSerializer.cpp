#include "assetSerializer.h"
#include <iosfwd>
#include <fstream>
#include <iostream>
#include <assert.h>

bool AssetSerializer::GetFileTypeString(const std::string filePath, std::string& fileTypeString)
{
	std::string firstLine;
	std::ifstream fileData(filePath);
	if (!fileData.is_open())
	{
		std::cout << "ERROR : Failed to open file " << filePath << std::endl;
		assert(fileData.is_open());
	}
	if (std::getline(fileData, firstLine))
	{
		fileTypeString = "";
		std::string fieldValue;
		if (GetFieldValue("###### GLEngineAsset", firstLine, fieldValue))
		{
			for (unsigned int fieldIndex = 0; fieldIndex < fieldValue.size() && fieldValue[fieldIndex] != '#' && fieldValue[fieldIndex] != ' '; ++fieldIndex)
			{
				fileTypeString += fieldValue[fieldIndex];
			}
			return true;
		}
	}
	std::cout << "WARNING : " << filePath << " is not an asset file " << std::endl;
	fileData.close();
	return false;
}

std::vector<std::string> AssetSerializer::GetFilePropertyNames(const std::string filePath)
{
	std::vector<std::string> outPropertyNames = {};
	std::string currentLine;
	std::ifstream fileData(filePath);
	unsigned int foundFilesCount = 0;
	if (!fileData.is_open())
	{
		std::cout << "ERROR : Failed to open file " << filePath << std::endl;
		assert(fileData.is_open());
	}
	while (std::getline(fileData, currentLine))
	{
		std::string fieldName;
		if (ExtractFieldName(fieldName, currentLine))
		{
			outPropertyNames.push_back(fieldName);
		}
	}
	fileData.close();
	return outPropertyNames;
}

bool AssetSerializer::ExtractFieldName(std::string& fieldName, const std::string lineString)
{
	if (lineString.size() <= 0) return false;
	if (lineString[0] != '#') return false;
	fieldName = "";
	for (unsigned int charIndex = 1; charIndex < lineString.size() && lineString[charIndex] != ' ' && lineString[charIndex] != ':' && lineString[charIndex] != '='; ++charIndex)
	{
		fieldName += lineString[charIndex];
	}
	return true;
}

bool AssetSerializer::IsField(const std::string fieldName, const std::string lineString)
{
	if (lineString.size() == 0) return false;
	if (fieldName.size() >= lineString.size()) return false;
	if (lineString[0] != '#') return false;

	for (unsigned int charIndex = 0; charIndex < fieldName.size() - 1; ++charIndex)
	{
		if (fieldName[charIndex] != lineString[charIndex + 1]) return false;
	}
	return true;
}


bool AssetSerializer::GetFieldValue(const std::string fieldName, const std::string lineString, std::string& fieldValue)
{
	if (fieldName.size() >= lineString.size()) return false;
	fieldValue = "";
	if (!IsField(fieldName, lineString)) return false;

	unsigned int charIndex = 0;

	for (charIndex = fieldName.size() + 1; charIndex < lineString.size() && (lineString[charIndex] == ' ' || lineString[charIndex] == '=' || lineString[charIndex] == ':' || lineString[charIndex] == '#'); ++charIndex) { }
	for (unsigned int lineCharIndex = charIndex; lineCharIndex < lineString.size(); ++lineCharIndex)
	{
		fieldValue += lineString[lineCharIndex];
	}
	return true;
}

std::vector<SPropertyValue> AssetSerializer::ReadFields(const std::string filePath, std::vector<std::string> fieldNames)
{
	std::vector<SPropertyValue> outPropertiesValues = {};
	std::string currentLine;
	std::ifstream fileData(filePath);
	unsigned int foundFilesCount = 0;
	if (!fileData.is_open())
	{
		std::cout << "ERROR : Failed to open file " << filePath << std::endl;
		assert(fileData.is_open());
	}
	while (std::getline(fileData, currentLine))
	{
		for (const auto& searchedField : fieldNames)
		{
			if (IsField(searchedField, currentLine))
			{
// 				outPropertiesValues.push_back(SPropertyValue(searchedField, currentLine));
// 				foundFilesCount++;
				break;
			}
		}
		if (fieldNames.size() <= foundFilesCount) break;
	}
	fileData.close();
	return outPropertiesValues;
}

std::string AssetSerializer::ReadField(const std::string filePath, const std::string searchedField, bool bAssertIfFailed)
{
	std::string currentLine;
	std::ifstream fileData(filePath);
	if (!fileData.is_open())
	{
		std::cout << "ERROR : Failed to open file " << filePath << std::endl;
		assert(fileData.is_open());
	}
	while (std::getline(fileData, currentLine))
	{
		if (IsField(searchedField, currentLine))
		{
			fileData.close();
			std::string fieldValue;
			GetFieldValue(searchedField, currentLine, fieldValue);
			return fieldValue;
		}
	}
	fileData.close();
	if (bAssertIfFailed)
	{
		std::cout << "ERROR : Failed to find field " << searchedField << " on asset " << filePath << std::endl;
		assert("Failed to find field ");
	}
	return "";
}

bool AssetSerializer::WriteField(const std::string filePath, const std::string fieldName, const std::string value)
{
	std::ifstream inputAssetFile(filePath);
	std::vector<std::string> fileLines = {};
	std::string input;

	while (std::getline(inputAssetFile, input))
		fileLines.push_back(input);

	inputAssetFile.close();

	bool bFoundField = false;
	for (int i = fileLines.size() - 1; i >= 0; --i)
	{
		std::string foundFieldName;
		if (bFoundField && ExtractFieldName(foundFieldName, fileLines[i]))
		{
			if (fieldName == foundFieldName)
			{
				fileLines.erase(fileLines.begin() + i);
			}
		}
		if (!bFoundField && IsField(fieldName, fileLines[i]))
		{
			fileLines[i] = "#" + fieldName + " = " + value;
			bFoundField = true;
		}

	}
	if (!bFoundField)
	{
		fileLines.push_back("#" + fieldName + " = " + value);
	}


	std::ofstream outputAssetFile(filePath);

	for (auto const& line : fileLines)
		outputAssetFile << line << std::endl;
	outputAssetFile.close();




	return true;
}

std::ifstream* AssetSerializer::BeginRead(std::string filePath)
{
	return new std::ifstream(filePath, std::ios::in | std::ios::binary);
}

void AssetSerializer::EndRead(std::ifstream* filePath)
{
	filePath->close();
}

std::ofstream* AssetSerializer::BeginWrite(std::string filePath)
{
	return new std::ofstream(filePath, std::ios::out | std::ios::binary);
}

void AssetSerializer::EndWrite(std::ofstream* filePath)
{
	filePath->close();
}
