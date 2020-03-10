
#pragma once

#include <EnginePCH.h>

#define GPROPERTY(var) GPropertyLink(&var, sizeof(var), #var, typeid(var).name(), EPropertyType::PT_Var)
#define REGISTER_PROPERTY(prop) RegisterProperty(GPROPERTY(prop))
#define GPROPERTYLIST(var) GPropertyLink(&var, sizeof(var), #var, typeid(var).name(), EPropertyType::PT_List)
#define REGISTER_PROPERTYLIST(prop) RegisterProperty(GPROPERTYLIST(prop))

enum class EPropertyType
{
	PT_Var = 0,
	PT_List = 1
};


struct GPropertyLink
{
private:

	std::string propertyName;
	std::string propertyType;
	EPropertyType propertyStructure;
	void* pointerTovar;
	unsigned int propertyLength;

	void SerializeV1(const char* result, unsigned int& size) const;
	void ComputeSizeV1(unsigned int& size) const;

public:

	GPropertyLink(void* inPointerTovar, const unsigned int& inLength, const std::string& inPropertyName, const std::string& type, const EPropertyType& inPropertyStructure) :
		pointerTovar(inPointerTovar),
		propertyLength(inLength),
		propertyName(inPropertyName),
		propertyType(type),
		propertyStructure(inPropertyStructure)
	{}

	virtual ~GPropertyLink() {}

	FORCEINLINE std::string GetName() const { return propertyName; }

	bool operator==(const GPropertyLink& inOther) const
	{
		return std::string(inOther.propertyName) == std::string(propertyName);
	}

	void Serialize(const char* result, unsigned int& size);
};