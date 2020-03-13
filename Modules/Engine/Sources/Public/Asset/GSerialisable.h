
#pragma once

#include <EnginePCH.h>

struct GProperty;
struct SAssetWriter;
struct SAssetReader;

class GSerialisable
{
	std::map<std::string, GProperty> registeredProperties;
	
protected:

	void RegisterProperty(const GProperty& inProperty);

public:

	virtual void RegisterProperties() {}

	void Serialize(char*& outData, unsigned int& outLength);
	void Deserialize(char*& inData);

	GProperty* GetProperty(std::string propertyName);

	GSerialisable() {}
	virtual ~GSerialisable() {}
};

class GSerialisableTest : public GSerialisable
{
public:
	int prop1 = 12;
	double prop2 = 4.8;
	float prop3 = 3.5f;
	virtual void RegisterProperties() override;

	GSerialisableTest() : GSerialisable() {}
};