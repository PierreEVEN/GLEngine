#include "worldInteraction.h"
#include "vector3.h"
#include "../Mesh/meshSectionComponent.h"
#include <bullet3D/btBulletDynamicsCommon.h>
#include <bullet3D/LinearMath/btIDebugDraw.h>
#include "../Physic/primitiveComponent.h"
#include "../Mesh/staticMeshComponent.h"
#include "../Camera/camera.h"
#include "../Mesh/staticMesh.h"
#include "../Engine/debugerTool.h"
#include "../Physic/physicDebugViewer.h"
#include "../World/scene.h"


SHitResult WorldInteractionLibrary::TraceScene(Scene* traceScene, SVector3 posStart, SVector3 posEnd, bool bDrawDebugTrace, SVector3 traceColor)
{
	ProfileStat("TraceMesh");
	SHitResult result;


	if (bDrawDebugTrace)
	{
		traceScene->GetDebugDrawer()->DrawDebugLine(posStart, posEnd, traceColor);
	}

	/** Get elements under trace from sphere bounds */
	std::vector<const MeshSectionComponent*> alignedComponents = {};
	for (SceneComponentIterator<StaticMeshComponent> ite(traceScene); ite; ite++)
	{
		BoxSphereBound objectBounds = ite->GetWorldBounds();
		if (GetPointDistanceToSegment(posStart, posEnd, objectBounds.GetOrigin()) < objectBounds.GetBoundRadius())
		{
			for (const auto& section : ite->sections)
			{
				objectBounds = section->GetWorldBounds();

				if (GetPointDistanceToSegment(posStart, posEnd, objectBounds.GetOrigin()) < objectBounds.GetBoundRadius())
				{
					alignedComponents.push_back(section);
				}
			}
		}
	}
	if (alignedComponents.size() <= 0) return result;
	for (unsigned int i = alignedComponents.size() - 1; i > 0; --i)
	{
		for (unsigned int j = 1; j <= i; ++j)
		{
			if ((alignedComponents[j]->GetWorldBounds().GetOrigin() - posStart).GetLength() > (alignedComponents[j - 1]->GetWorldBounds().GetOrigin() - posStart).GetLength())
			{
				const MeshSectionComponent* replacedSection = alignedComponents[j];
				alignedComponents[j] = alignedComponents[j - 1];
				alignedComponents[j - 1] = replacedSection;
			}
		}
	}

	for (const MeshSectionComponent* section : alignedComponents)
	{
		/** compute section space trace direction */
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, SVector3(1.f / section->GetScale3D().x, 1.f / section->GetScale3D().y, 1.f / section->GetScale3D().z).ToGLVector());
		model = glm::rotate(model, -glm::radians(section->GetAngle()), section->GetForwardVector());
		model = glm::translate(model, -section->GetLocation().ToGLVector());
		glm::vec3 localTraceStart = glm::translate(model, posStart.ToGLVector())[3];
		glm::vec3 localTraceEnd = glm::translate(model, posEnd.ToGLVector())[3];
		glm::vec3 localTraceDirection = localTraceEnd - localTraceStart;
		const StaticMeshSection* sectionData = section->GetSectionData();
		const unsigned int verticesCount = section->GetSectionData()->sectionIndices.size() / 3;

		for (unsigned int i = 0; i < verticesCount; ++i)
		{
			/** get triangle vertices */
			unsigned int indicePos = i * 3;
			glm::vec3 verticeA(sectionData->sectionVertices[sectionData->sectionIndices[indicePos]].Position);
			glm::vec3 verticeB(sectionData->sectionVertices[sectionData->sectionIndices[++indicePos]].Position);
			glm::vec3 verticeC(sectionData->sectionVertices[sectionData->sectionIndices[++indicePos]].Position);

			/** get triangle normal */
			glm::vec3 segmentAB = (verticeB - verticeA);
			glm::vec3 segmentBC = (verticeC - verticeB);
			glm::vec3 faceNormal = CrossProduct(segmentAB, segmentBC);

			/** ignore back faces */
			if (DotProduct(faceNormal, localTraceDirection) > 0) continue;

			/** check if local trace is under segment AB */
			glm::vec3 resultSegmentAB = CrossProduct(segmentAB, localTraceDirection);
			glm::vec3 directionSegmentA = (verticeA - localTraceStart);
			if (DotProduct(resultSegmentAB, directionSegmentA) > 0) continue;

			/** check if local trace is under segment BC */
			glm::vec3 resultSegmentBC = CrossProduct(segmentBC, localTraceDirection);
			glm::vec3 directionSegmentB = (verticeB - localTraceStart);
			if (DotProduct(resultSegmentBC, directionSegmentB) > 0) continue;

			/** check if local trace is under segment CA */
			glm::vec3 segmentCA = (verticeA - verticeC);
			glm::vec3 resultSegmentCA = CrossProduct(segmentCA, localTraceDirection);
			glm::vec3 directionSegmentC = (verticeC - localTraceStart);
			if (DotProduct(resultSegmentCA, directionSegmentC) > 0) continue;

			/** retrieve triangle data */
			result.bHitComponent = true;
			result.hitSection = (MeshSectionComponent*)section;
			if (bDrawDebugTrace)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, section->GetLocation().ToGLVector());
				model = glm::rotate(model, glm::radians(section->GetAngle()), section->GetForwardVector());
				model = glm::scale(model, section->GetScale3D().ToGLVector());

				verticeA = glm::translate(model, verticeA)[3];
				verticeB = glm::translate(model, verticeB)[3];
				verticeC = glm::translate(model, verticeC)[3];

				traceScene->GetDebugDrawer()->drawTriangle(
					SVector3(verticeA).ToBulletVector(),
					SVector3(verticeB).ToBulletVector(),
					SVector3(verticeC).ToBulletVector(),
					btVector3(1, 0, 0),
					1);
				traceScene->GetDebugDrawer()->drawBox(
					section->GetWorldBounds().GetBoxMin().ToBulletVector(),
					section->GetWorldBounds().GetBoxMax().ToBulletVector(),
					btVector3(0.7f, 0.5f, 0));
				return result;
				//@TODO Add result face sorting to only keep front face
			}
		}
	}
	return result;
}

double WorldInteractionLibrary::GetPointDistanceToLine(SVector3 lineDirection, SVector3 lineOrigin, SVector3 pointLocation)
{
	SVector3 lineToPointVector = lineOrigin - pointLocation;
	SVector3 surfaceNormal = CrossProduct(lineDirection, lineToPointVector);
	SVector3 Normal = CrossProduct(surfaceNormal, lineDirection);
	Normal.Normalize();
	return abs(DotProduct(Normal, lineToPointVector));
}

double WorldInteractionLibrary::GetPointDistanceToSegment(SVector3 posA, SVector3 posB, SVector3 pointLocation)
{
	SVector3 segmentDirection = posB - posA;
	SVector3 targetDirectionA = pointLocation - posA;
	if (DotProduct(segmentDirection, targetDirectionA) < 0)
	{
		return (pointLocation - posA).GetLength();
	}
	SVector3 targetDirectionB = pointLocation - posB;
	segmentDirection *= -1;
	if (DotProduct(segmentDirection, targetDirectionB) < 0)
	{
		return (pointLocation - posB).GetLength();
	}
	return GetPointDistanceToLine((posA - posB).Normalize(), posA, pointLocation);
}

double WorldInteractionLibrary::GetSegmentDistanceToLine(SVector3 lineOrigin, SVector3 lineDirection, SVector3 segmentPosA, SVector3 segmentPosB)
{
	SVector3 segmentDirection = segmentPosB - segmentPosA;
	SVector3 distanceDirection = CrossProduct(lineDirection, segmentDirection);
	SVector3 directionToLine = lineOrigin - segmentPosA;
	return abs(DotProduct(directionToLine, distanceDirection));
}
