#include "Asset/GSerialisable.h"
#include "Asset/GProperty.h"

void GSerialisable::RegisterProperty(const GProperty& inProperty) {
	registeredProperties.insert({ inProperty.GetName(), inProperty });
}

void GSerialisable::RegisterProperties()
{
}

GProperty* GSerialisable::GetProperty(std::string propertyName)
{
	return &registeredProperties.find(propertyName)->second;
}
