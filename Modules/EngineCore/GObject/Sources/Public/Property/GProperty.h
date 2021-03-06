
#pragma once

#include <CorePCH.h>

#define GPROPERTY(var) GProperty(&var, sizeof(var), #var, typeid(var).name(), EPropertyType::PT_Var)
#define REGISTER_PROPERTY(prop) RegisterProperty(GPROPERTY(prop))
#define GPROPERTYLIST(var) GProperty(&var, sizeof(var), #var, typeid(var).name(), EPropertyType::PT_List)
#define REGISTER_PROPERTYLIST(prop) RegisterProperty(GPROPERTYLIST(prop))

enum class EPropertyType
{
	PT_Var = 0,
	PT_Tab = 1,
	PT_List = 2
};

struct GProperty
{
private:

	/** Property string name */
	std::string propertyName;
	/** Property string type name */
	std::string propertyType;
	/** Property type */
	EPropertyType propertyStructure;
	/** Property memory size */
	unsigned int propertyLength;
	/** Pointer to property */
	void* pointerTovar = nullptr;

	void ComputeSizeV1(unsigned int& size) const;
	void SerializeV1(char*& result) const;
	void DeserializeV1(char*& source);

public:

	GProperty(void* inPointerTovar, const unsigned int& inLength, const std::string& inPropertyName, const std::string& type, const EPropertyType& inPropertyStructure) :
		pointerTovar(inPointerTovar),
		propertyLength(inLength),
		propertyName(inPropertyName),
		propertyType(type),
		propertyStructure(inPropertyStructure)
	{}

	GProperty(char*& source) { Deserialize(source); }

	virtual ~GProperty() {}

	FORCEINLINE std::string GetName() const { return propertyName; }

	bool operator==(const GProperty& inOther) const
	{
		return std::string(inOther.propertyName) == std::string(propertyName);
	}

	void Serialize(char*& result) const;
	void GetSize(unsigned int& inSize) const;
	void Deserialize(char*& source);

	std::string ToString() const;

	static std::string GetPropertyName(char*& data);

	template<typename T> T* Get() { return (T*)pointerTovar; }
};