
#pragma once

#include <EnginePCH.h>

class GSerialisable;

class GProperty
{
private:
	std::string PropertyName;
	GSerialisable* owner;
	void* value;
	unsigned int length;

public:

	GProperty(void* property, const unsigned int& length, const std::string& propertyName) {}
	virtual ~GProperty() {}
};

struct GPropertyLink
{

};