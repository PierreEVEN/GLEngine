
#pragma once

#include <EnginePCH.h>

class GProperty;
struct GPropertyLink;

class GSerialisable
{
	std::map<std::string, GPropertyLink*> registeredProperties;
	
	unsigned int testProperty;


protected:

	template <typename T>
	void RegisterProperty(T* inProperty) {}

public:

	GSerialisable()
	{
		RegisterProperty<unsigned int>(&testProperty);
	}


	virtual ~GSerialisable() {}

	std::forward_list<GProperty*> Serialize() {}
	void Deserialize(const std::forward_list<GProperty*>& inProperties) {}
};