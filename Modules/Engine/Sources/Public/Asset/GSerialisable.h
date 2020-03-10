
#pragma once

#include <EnginePCH.h>

struct GPropertyLink;

class GSerialisable
{
	std::forward_list<GPropertyLink> registeredProperties;
	
	unsigned int testProperty;
	
protected:

	void RegisterProperty(const GPropertyLink& inProperty);

	virtual void RegisterProperties();

public:

	GSerialisable() {}
	virtual ~GSerialisable() {}
};