#include "Mesh/MeshData.h"
#include <Scene/SceneMeshProxy.h>

void MeshSectionData::ComputeBounds()
{
	SVector3 min;
	SVector3 max;
	bool bSet = false;
	for (unsigned int verticeIndex = 0; verticeIndex < verticesCount; ++verticeIndex)
	{
		SVertex* changedVertice = &sectionVertices[verticeIndex];
		if (changedVertice->Position.x > max.x || !bSet) max.x = changedVertice->Position.x;
		if (changedVertice->Position.y > max.y || !bSet) max.y = changedVertice->Position.y;
		if (changedVertice->Position.z > max.z || !bSet) max.z = changedVertice->Position.z;

		if (changedVertice->Position.x < min.x || !bSet) min.x = changedVertice->Position.x;
		if (changedVertice->Position.y < min.y || !bSet) min.y = changedVertice->Position.y;
		if (changedVertice->Position.z < min.z || !bSet) min.z = changedVertice->Position.z;
		bSet = true;
	}
	localBounds = BoxSphereBound(min, max);
}

void SProxyLodGroup::CopyDataToSceneMeshProxy(SceneMeshProxy* inProxy)
{
	CHECK_RENDER_THREAD;
	bHasDataBeenChangedRenderThread = false;
	inProxy->UpdateDataRenderThread(this);
}