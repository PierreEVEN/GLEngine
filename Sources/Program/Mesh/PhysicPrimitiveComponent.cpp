#include "PhysicPrimitiveComponent.h"

PhysicPrimitiveComponent::PhysicPrimitiveComponent(World* inWorld)
	: PrimitiveComponent(inWorld)
{

}

PhysicPrimitiveComponent::PhysicPrimitiveComponent(World* inWorld, StaticMeshSection* inStaticMeshSection)
	: PrimitiveComponent(inWorld, inStaticMeshSection)
{
	Init();
}
