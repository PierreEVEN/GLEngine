
#include <Mesh/staticMeshComponent.h>
#include <Mesh/meshSectionComponent.h>
#include <Mesh/StaticMesh.h>
#include <Physic/physicDebugViewer.h>
#include <Scene/scene.h>


void StaticMeshComponent::BuildMesh()
{
	for (auto& section : meshData->GetSections())
	{
		MeshSectionComponent* newSection = new MeshSectionComponent(GetScene(), this, section);
		newSection->SetLocation(GetLocation());
		sections.push_back(newSection);
	}
	for (unsigned int i = 0; i < tmpMaterialOveride.size() && i < sections.size(); ++i)
	{
		sections[i]->materialOverride = tmpMaterialOveride[i];
	}
	RebuildLocalBounds();
}

StaticMeshComponent::StaticMeshComponent(Scene* inScene, AStaticMesh* inMeshData, std::vector<Material*> newMaterialOverride /*= {}*/)
	: PrimitiveComponent(inScene)
{
	if (!inMeshData) return;
	tmpMaterialOveride = newMaterialOverride;
	meshData = inMeshData;
	if (meshData->AreDataLoaded())
	{
		BuildMesh();
	}
	else
	{
		meshData->BindOnAssetLoaded(this, &StaticMeshComponent::BuildMesh);
		meshData->CheckData(true);
	}
}

StaticMeshComponent::StaticMeshComponent(Scene* inScene, AStaticMesh* inMeshData, std::vector<Material*> newMaterialOverride /*= {}*/, btCollisionShape* physicBody, bool bSimulatePhysics)
	: StaticMeshComponent(inScene, inMeshData, newMaterialOverride)
{
}

StaticMeshComponent::~StaticMeshComponent()
{
	for (auto& section : sections)
	{
		section->DestroyComponent();
	}
	sections.clear();
	meshData = nullptr;
}

void StaticMeshComponent::Tick()
{
	PrimitiveComponent::Tick();
}

void StaticMeshComponent::RebuildTransformData()
{
	PrimitiveComponent::RebuildTransformData();
	for (auto& section : sections)
	{
		section->SetLocation(GetLocation());
		section->SetRotation(GetRotation());
		section->SetScale3D(GetScale3D());
	}
}

void StaticMeshComponent::RebuildLocalBounds()
{
	localBounds = meshData->bounds;
	PrimitiveComponent::RebuildLocalBounds();
}

void StaticMeshComponent::DrawEditorWindow()
{
	PrimitiveComponent::DrawEditorWindow();
	GetScene()->GetDebugDrawer()->drawBox(GetWorldBounds().GetBoxMin().ToBulletVector(), GetWorldBounds().GetBoxMax().ToBulletVector(), btVector3(1.0f, 0.8f, 0.1f));
}
