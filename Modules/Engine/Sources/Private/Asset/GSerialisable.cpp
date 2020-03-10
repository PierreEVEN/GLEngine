#include "Asset/GSerialisable.h"
#include "Asset/GProperty.h"

void GSerialisable::RegisterProperty(const GPropertyLink& inProperty)
{
	for (const GPropertyLink& prop : registeredProperties)
	{
		if (prop == inProperty) GLogAssert("Error, already registered property named " + inProperty.GetName());
	}
	registeredProperties.push_front(inProperty);
}

void GSerialisable::RegisterProperties()
{
}