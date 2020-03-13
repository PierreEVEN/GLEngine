
#pragma once

#include <EnginePCH.h>

#define GPROPERTY(var) GPropertyV1(&var, sizeof(var), #var, typeid(var).name(), EPropertyType::PT_Var)
#define REGISTER_PROPERTY(prop) RegisterProperty(GPROPERTY(prop))
#define GPROPERTYLIST(var) GPropertyV1(&var, sizeof(var), #var, typeid(var).name(), EPropertyType::PT_List)
#define REGISTER_PROPERTYLIST(prop) RegisterProperty(GPROPERTYLIST(prop))

enum class EPropertyType {
	PT_Var = 0,
	PT_List = 1
};


struct GProperty {
protected:

	std::string propertyName;
	std::string propertyType;
	EPropertyType propertyStructure;
	unsigned int propertyLength;
	void* pointerTovar = nullptr;

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

	bool operator==(const GProperty& inOther) const	{ return std::string(inOther.propertyName) == std::string(propertyName); }

	virtual void Serialize(char*& result) const {}
	virtual void GetSize(unsigned int& inSize) const {}
	virtual void Deserialize(char*& source) {}
	virtual std::string ToString() const { return ""; }

	static std::string GetPropertyName(char*& data);

	template<typename T> T* Get() { return (T*)pointerTovar; }
};

struct GPropertyV1 : public GProperty
{
public:

	GPropertyV1(void* inPointerTovar, const unsigned int& inLength, const std::string& inPropertyName, const std::string& type, const EPropertyType& inPropertyStructure) :
		GProperty(inPointerTovar, inLength, inPropertyName, type, inPropertyStructure)
	{}

	GPropertyV1(char*& source) : GProperty(source) { }
	
	virtual void Serialize(char*& result) const override;
	virtual void GetSize(unsigned int& inSize) const override;
	virtual void Deserialize(char*& source) override;
	virtual std::string ToString() const override;
};