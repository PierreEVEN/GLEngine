#include "Asset/GProperty.h"


/* structure : nameLength(uint) | name(char*) | typeLength(uint) | type(char*) | propertyStructure(char) | propertyLength(uint) | property(char* ) */

void GPropertyLink::SerializeV1(const char* result, unsigned int& size) const
{

}

void GPropertyLink::ComputeSizeV1(unsigned int& size) const
{
	size = sizeof(unsigned int);					//nameLength
	size += propertyName.size() * sizeof(char);		//name
	size += sizeof(unsigned int);					//typeLength
	size += propertyType.size() * sizeof(char);		//type
	size += sizeof(char);							//structure
	size += propertyLength;							//propertyLength
	size += propertyLength * sizeof(char);			//property
}

void GPropertyLink::Serialize(const char* result, unsigned int& size)
{
}