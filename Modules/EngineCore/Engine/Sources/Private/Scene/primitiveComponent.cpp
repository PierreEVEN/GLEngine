

#include <Physic/physicDebugViewer.h>
#include <Scene/primitiveComponent.h>
#include <Scene/scene.h>

bool bDrawSphereBounds = false;
bool bDrawBoxBounds = false;

BoxSphereBound::BoxSphereBound(SVector3 min, SVector3 max)
{
	boxMin = min;
	boxMax = max;
	boundRadius = (float)(GetOrigin() - boxMin).GetLength();
}

SVector3 BoxSphereBound::GetOrigin() const
{
	return SVector3((boxMin.x + boxMax.x) / 2.0, (boxMin.y + boxMax.y) / 2, (boxMin.z + boxMax.z) / 2.0);
}

PrimitiveComponent::PrimitiveComponent(Scene* inScene)
	: SceneComponent(inScene)
{
}

PrimitiveComponent::~PrimitiveComponent()
{
}

void PrimitiveComponent::Render()
{
	SceneComponent::Render();

	if (GetScene()->GetDebugDrawFlag() == ESceneDebugDrawFlag::DrawBoxBounds)
	{
		GetScene()->GetDebugDrawer()->drawBox(GetWorldBounds().GetBoxMin().ToBulletVector(), GetWorldBounds().GetBoxMax().ToBulletVector(), SVector3(0, .4f, 1).ToBulletVector());
	}
	else if (GetScene()->GetDebugDrawFlag() == ESceneDebugDrawFlag::DrawSphereBounds)
	{
		GetScene()->GetDebugDrawer()->drawSphere(GetWorldBounds().GetOrigin().ToBulletVector(), GetWorldBounds().GetBoundRadius(), SVector3(.5f, 1, 0).ToBulletVector());
	}

	if (DoesDrawBoxBounds())
	{
		GetScene()->GetDebugDrawer()->drawBox(GetWorldBounds().GetBoxMin().ToBulletVector(), GetWorldBounds().GetBoxMax().ToBulletVector(), SVector3(0, 1, 0).ToBulletVector());
	}
	if (DoesDrawSphereBounds())
	{
		GetScene()->GetDebugDrawer()->drawSphere(GetWorldBounds().GetOrigin().ToBulletVector(), GetWorldBounds().GetBoundRadius(), SVector3(0, 0, 1).ToBulletVector());
	}
}

void PrimitiveComponent::RebuildTransformData()
{
	SceneComponent::RebuildTransformData();
	RebuildWorldBounds();
}

void PrimitiveComponent::RebuildWorldBounds()
{
	SVector3 min = GetLocalBounds().GetBoxMin();
	SVector3 max = GetLocalBounds().GetBoxMax();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, GetLocation().ToGLVector());
	model = model * glm::mat4_cast(glm::quat((float)GetRotation().w, (float)GetRotation().x, (float)GetRotation().y, (float)GetRotation().z));
	model = glm::scale(model, GetScale3D().ToGLVector());
	glm::mat4 cubePos[8];
	cubePos[0] = glm::translate(model, SVector3(min.x, min.y, min.z).ToGLVector());
	cubePos[1] = glm::translate(model, SVector3(min.x, min.y, max.z).ToGLVector());
	cubePos[2] = glm::translate(model, SVector3(min.x, max.y, max.z).ToGLVector());
	cubePos[3] = glm::translate(model, SVector3(max.x, max.y, max.z).ToGLVector());
	cubePos[4] = glm::translate(model, SVector3(max.x, max.y, min.z).ToGLVector());
	cubePos[5] = glm::translate(model, SVector3(max.x, min.y, min.z).ToGLVector());
	cubePos[6] = glm::translate(model, SVector3(max.x, min.y, max.z).ToGLVector());
	cubePos[7] = glm::translate(model, SVector3(min.x, max.y, min.z).ToGLVector());

	SVector3 worldMin = SVector3(cubePos[0][3]);
	SVector3 worldMax = SVector3(cubePos[0][3]);
	for (const auto& pos : cubePos)
	{
		if (pos[3].x > worldMax.x) worldMax.x = pos[3].x;
		if (pos[3].y > worldMax.y) worldMax.y = pos[3].y;
		if (pos[3].z > worldMax.z) worldMax.z = pos[3].z;

		if (pos[3].x < worldMin.x) worldMin.x = pos[3].x;
		if (pos[3].y < worldMin.y) worldMin.y = pos[3].y;
		if (pos[3].z < worldMin.z) worldMin.z = pos[3].z;
	}

	worldBounds = BoxSphereBound(worldMin, worldMax);
}

void PrimitiveComponent::DrawEditorWindow()
{
	SceneComponent::DrawEditorWindow();
}

void PrimitiveComponent::SetDrawSphereBounds(bool bInDrawDebug)
{
	bDrawSphereBounds = bInDrawDebug;
}

void PrimitiveComponent::SetDrawBoxBounds(bool bInDrawDebug)
{
	bDrawBoxBounds = bInDrawDebug;
}

bool PrimitiveComponent::DoesDrawSphereBounds()
{
	return bDrawSphereBounds;
}

bool PrimitiveComponent::DoesDrawBoxBounds()
{
	return bDrawBoxBounds;
}