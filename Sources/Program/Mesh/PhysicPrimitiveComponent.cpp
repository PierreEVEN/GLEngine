#include "PhysicPrimitiveComponent.h"

PhysicPrimitiveComponent::PhysicPrimitiveComponent(World* inWorld)
	: MeshSectionComponent(inWorld)
{

}

PhysicPrimitiveComponent::PhysicPrimitiveComponent(World* inWorld, StaticMeshSection* inStaticMeshSection)
	: MeshSectionComponent(inWorld, inStaticMeshSection)
{
	Init();
}
