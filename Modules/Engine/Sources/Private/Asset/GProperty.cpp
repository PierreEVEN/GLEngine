#include "Asset/GProperty.h"


/************************************************************************/
/* Serialization V1.0                                                   */
/************************************************************************/
/* structure : nameLength(uint) | name(char*) | typeLength(uint) | type(char*) | propertyStructure(char) | propertyLength(uint) | property(char* ) */

void GPropertyV1::GetSize(unsigned int& size) const
{
	size = 0;
	size += sizeof(unsigned int);					//structure
	size += sizeof(unsigned int);					//nameLength
	size += (unsigned int)propertyName.size() + 1;				//name
	size += sizeof(unsigned int);					//typeLength
	size += (unsigned int)propertyType.size() + 1;				//type
	size += sizeof(unsigned int);					//propertyLength
	size += propertyLength;							//property
}

void GPropertyV1::Serialize(char*& result) const
{
	unsigned int size;
	GetSize(size);
	result = (char*)malloc(size);
	unsigned int currentOffset = 0;

	/** structure */
	memcpy(&result[currentOffset], &propertyStructure, sizeof(unsigned int));
	currentOffset += sizeof(unsigned int);

	/** nameLength */
	unsigned int nameLength = (unsigned int)propertyName.size() + 1;
	memcpy(&result[currentOffset], &nameLength, sizeof(unsigned int));
	currentOffset += sizeof(unsigned int);

	/** name */
	memcpy(&result[currentOffset], propertyName.data(), nameLength * sizeof(char));
	currentOffset += nameLength;

	/** typeLength */
	unsigned int typeLength = (unsigned int)propertyType.size() + 1;
	memcpy(&result[currentOffset], &typeLength, sizeof(unsigned int));
	currentOffset += sizeof(unsigned int);

	/** type */
	memcpy(&result[currentOffset], propertyType.data(), typeLength * sizeof(char));
	currentOffset += typeLength;

	/** propertyLength */
	memcpy(&result[currentOffset], &propertyLength, sizeof(unsigned int));
	currentOffset += sizeof(unsigned int);

	/** property */
	memcpy(&result[currentOffset], pointerTovar, propertyLength * sizeof(char));
	currentOffset += propertyLength;

	if (size != currentOffset) GLogAssert("Error during offset calculation : " + std::to_string(size) + " | " + std::to_string(currentOffset));
}


void GPropertyV1::Deserialize(char*& source)
{
	unsigned int currentOffset = 0;

	/** structure */
	memcpy((unsigned int*)(&propertyStructure), &source[currentOffset], sizeof(unsigned int));
	currentOffset += sizeof(unsigned int);

	/** nameLength */
	unsigned int nameLength;
	memcpy(&nameLength, &source[currentOffset], sizeof(unsigned int));
	currentOffset += sizeof(unsigned int);

	/** name */
	char* name = (char*)malloc(nameLength);
	memcpy(name, &source[currentOffset], nameLength);
	currentOffset += nameLength;
	propertyName = name;

	/** typeLength */
	unsigned int typeLength;
	memcpy(&typeLength, &source[currentOffset], sizeof(unsigned int));
	currentOffset += sizeof(unsigned int);

	/** type */
	char* type = (char*)malloc(typeLength);;
	memcpy(type, &source[currentOffset], typeLength);
	currentOffset += typeLength;
	propertyType = type;

	/** propertyLength */
	memcpy(&propertyLength, &source[currentOffset], sizeof(unsigned int));
	currentOffset += sizeof(unsigned int);

	/** property */
	//pointerTovar = (void*)malloc(propertyLength);
	memcpy(pointerTovar, &source[currentOffset], propertyLength);
	currentOffset += propertyLength;
}

std::string GPropertyV1::ToString() const
{
	return " (" + std::to_string((int)propertyStructure) + ") " + propertyType + " " + propertyName + " (" + std::to_string(propertyLength) + ") ";
}

std::string GProperty::GetPropertyName(char*& data)
{
	unsigned int currentOffset = 0;
	/** PropertyType */
	currentOffset += sizeof(unsigned int);

	/** NameLength */
	currentOffset += sizeof(unsigned int);

	/** Name */
	return &data[currentOffset];
}
