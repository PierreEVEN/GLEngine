#include <Property/GSerialisable.h>
#include <Property/GProperty.h>


void GSerialisable::RegisterProperty(const GProperty& inProperty) {
	registeredProperties.insert({ inProperty.GetName(), inProperty });
}

void GSerialisable::Serialize(char*& outData, unsigned int& outLength)
{
	outLength = 0;
	unsigned int currentOffset = 0;

	outLength += sizeof(unsigned int); //File length

	for (std::map<std::string, GProperty>::iterator it = registeredProperties.begin(); it != registeredProperties.end(); it++)
	{
		unsigned int propertyLength;
		it->second.GetSize(propertyLength);
		std::cout << "Serialize " << it->second.ToString() << std::endl;
		outLength += sizeof(unsigned int);			//propertyLength
		outLength += propertyLength;				//property
	}

	outData = (char*)malloc(outLength);

	memcpy(&outData[currentOffset], &outLength, sizeof(unsigned int));
	currentOffset += sizeof(unsigned int);

	for (std::map<std::string, GProperty>::iterator it = registeredProperties.begin(); it != registeredProperties.end(); it++)
	{
		char* localElemData;
		unsigned int propertyLength;
		it->second.Serialize(localElemData);
		it->second.GetSize(propertyLength);
		memcpy(&outData[currentOffset], &propertyLength, sizeof(unsigned int));
		currentOffset += sizeof(unsigned int);
		memcpy(&outData[currentOffset], localElemData, propertyLength);
		currentOffset += propertyLength;
		free(localElemData);
	}

	if (outLength != currentOffset) GLogAssert("Error during property serialization : " + std::to_string(outLength) + " | " + std::to_string(currentOffset));
}

void GSerialisable::Deserialize(char*& inData)
{
	unsigned int fileLength;
	unsigned int currentOffset = 0;
	memcpy(&fileLength, &inData[currentOffset], sizeof(unsigned int));
	currentOffset += sizeof(unsigned int);

	while (currentOffset < fileLength)
	{
		unsigned int propertyLength;
		memcpy(&propertyLength, &inData[currentOffset], sizeof(unsigned int));
		currentOffset += sizeof(unsigned int);

		char* data = &inData[currentOffset];// -> property size

		GetProperty(GProperty::GetPropertyName(data))->Deserialize(data);
		currentOffset += propertyLength;
	}
}

GProperty* GSerialisable::GetProperty(std::string propertyName)
{
	return &registeredProperties.find(propertyName)->second;
}

void GSerialisableTest::RegisterProperties()
{
	GSerialisable::RegisterProperties();
	REGISTER_PROPERTY(prop1);
	REGISTER_PROPERTY(prop2);
	REGISTER_PROPERTY(prop3);
}