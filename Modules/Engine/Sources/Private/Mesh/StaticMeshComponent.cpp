
#include <Mesh/staticMeshComponent.h>
#include <Mesh/StaticMesh.h>
#include <Physic/physicDebugViewer.h>
#include <Scene/scene.h>

StaticMeshComponent::StaticMeshComponent(Scene* inScene, AStaticMesh* inMeshData, std::vector<Material*> newMaterialOverride /*= {}*/)
	: MeshComponent(inScene)
{
	if (!inMeshData) return;
	meshData = inMeshData;
	drawPriority = DrawPriority_Normal;
}

StaticMeshComponent::~StaticMeshComponent() {}

void StaticMeshComponent::Render() {
	if (!bHasMeshDataBeenInitialized && meshData->AreDataLoaded())
	{
		bHasMeshDataBeenInitialized = true;

		ProxyMeshData.CopyFromAdditive(meshData->GetMeshData());
	}
	MeshComponent::Render();
}