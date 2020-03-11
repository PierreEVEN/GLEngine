
#pragma once

#include <EnginePCH.h>

struct GProperty;

class GSerialisable
{
	std::map<std::string, GProperty> registeredProperties;
	
protected:

	void RegisterProperty(const GProperty& inProperty);

	virtual void RegisterProperties();

public:

	GProperty* GetProperty(std::string propertyName);

	GSerialisable() {}
	virtual ~GSerialisable() {}
};