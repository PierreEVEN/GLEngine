#include "staticMeshComponent.h"
#include "staticMesh.h"
#include "meshSectionComponent.h"
#include "../World/world.h"

StaticMeshComponent::StaticMeshComponent(World* newWorld, StaticMesh* inMeshData, std::vector<Material*> newMaterialOverride /*= {}*/)
	: SceneComponent(newWorld)
{
	materialOverride = newMaterialOverride;
	meshData = inMeshData;
	
	for (auto& section : inMeshData->meshSections)
	{
		MeshSectionComponent* newSection = new MeshSectionComponent(newWorld, &section);
		newSection->SetLocation(GetLocation());
		sections.push_back(newSection);
	}
}

void StaticMeshComponent::SetLocation(glm::vec3 newLocation)
{
	SceneComponent::SetLocation(newLocation);
	for (auto& section : sections)
	{
		section->SetLocation(newLocation);
	}
}

void StaticMeshComponent::SetForwardVector(glm::vec3 newForwardVector)
{
	SceneComponent::SetForwardVector(newForwardVector);
	for (auto& section : sections)
	{
		section->SetForwardVector(newForwardVector);
	}
}

void StaticMeshComponent::SetAngle(float newAngle)
{
	SceneComponent::SetAngle(newAngle);
	for (auto& section : sections)
	{
		section->SetAngle(newAngle);
	}
}

void StaticMeshComponent::SetScale3D(glm::vec3 newScale3D)
{
	SceneComponent::SetScale3D(newScale3D);
	for (auto& section : sections)
	{
		section->SetScale3D(newScale3D);
	}
}
