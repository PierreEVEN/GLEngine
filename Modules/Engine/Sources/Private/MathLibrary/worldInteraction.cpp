#include <MathLibrary/worldInteraction.h>
#include <Mesh/staticMeshComponent.h>
#include <Mesh/StaticMesh.h>
#include <Mesh/meshSectionComponent.h>
#include <Physic/physicDebugViewer.h>
#include <Scene/scene.h>
#include <Mesh/MeshData.h>


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
		model = model * glm::mat4_cast(glm::quat((float)section->GetRotation().w, (float)section->GetRotation().x, (float)section->GetRotation().y, (float)section->GetRotation().z));
		model = glm::translate(model, -section->GetLocation().ToGLVector());
		glm::vec3 localTraceStart = glm::translate(model, posStart.ToGLVector())[3];
		glm::vec3 localTraceEnd = glm::translate(model, posEnd.ToGLVector())[3];
		glm::vec3 localTraceDirection = localTraceEnd - localTraceStart;
		const MeshSectionData* sectionData = section->GetSectionData();
		const unsigned int verticesCount = section->GetSectionData()->indicesCount / 3;

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
			glm::vec3 faceNormal = CrossProductMacro(segmentAB, segmentBC);

			/** ignore back faces */
			if (DotProductMacro(faceNormal, localTraceDirection) > 0) continue;

			/** check if local trace is under segment AB */
			glm::vec3 resultSegmentAB = CrossProductMacro(segmentAB, localTraceDirection);
			glm::vec3 directionSegmentA = (verticeA - localTraceStart);
			if (DotProductMacro(resultSegmentAB, directionSegmentA) > 0) continue;

			/** check if local trace is under segment BC */
			glm::vec3 resultSegmentBC = CrossProductMacro(segmentBC, localTraceDirection);
			glm::vec3 directionSegmentB = (verticeB - localTraceStart);
			if (DotProductMacro(resultSegmentBC, directionSegmentB) > 0) continue;

			/** check if local trace is under segment CA */
			glm::vec3 segmentCA = (verticeA - verticeC);
			glm::vec3 resultSegmentCA = CrossProductMacro(segmentCA, localTraceDirection);
			glm::vec3 directionSegmentC = (verticeC - localTraceStart);
			if (DotProductMacro(resultSegmentCA, directionSegmentC) > 0) continue;

			/** retrieve triangle data */
			result.bHitComponent = true;
			result.hitSection = (MeshSectionComponent*)section;
			if (bDrawDebugTrace)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, section->GetLocation().ToGLVector());
				model = model * glm::mat4_cast(glm::quat((float)section->GetRotation().w, (float)section->GetRotation().x, (float)section->GetRotation().y, (float)section->GetRotation().z));
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
	SVector3 surfaceNormal = CrossProductMacro(lineDirection, lineToPointVector);
	SVector3 Normal = CrossProductMacro(surfaceNormal, lineDirection);
	Normal.Normalize();
	return abs(DotProductMacro(Normal, lineToPointVector));
}

double WorldInteractionLibrary::GetPointDistanceToSegment(SVector3 posA, SVector3 posB, SVector3 pointLocation)
{
	SVector3 segmentDirection = posB - posA;
	SVector3 targetDirectionA = pointLocation - posA;
	if (DotProductMacro(segmentDirection, targetDirectionA) < 0)
	{
		return (pointLocation - posA).GetLength();
	}
	SVector3 targetDirectionB = pointLocation - posB;
	segmentDirection *= -1;
	if (DotProductMacro(segmentDirection, targetDirectionB) < 0)
	{
		return (pointLocation - posB).GetLength();
	}
	return GetPointDistanceToLine((posA - posB).Normalize(), posA, pointLocation);
}

double WorldInteractionLibrary::GetSegmentDistanceToLine(SVector3 lineOrigin, SVector3 lineDirection, SVector3 segmentPosA, SVector3 segmentPosB)
{
	SVector3 segmentDirection = segmentPosB - segmentPosA;
	SVector3 distanceDirection = CrossProductMacro(lineDirection, segmentDirection);
	SVector3 directionToLine = lineOrigin - segmentPosA;
	return abs(DotProductMacro(directionToLine, distanceDirection));
}

double WorldInteractionLibrary::GetSegmentDistanceFromSegment(SVector3 SegAPosA, SVector3 SegAPosB, SVector3 SegBPosA, SVector3 SegBPosB)
{
	SVector3   u = SegAPosB - SegAPosA;
	SVector3   v = SegBPosB - SegBPosA;
	SVector3   w = SegAPosA - SegBPosA;
	double    a = DotProductMacro(u, u);         // always >= 0
	double    b = DotProductMacro(u, v);
	double    c = DotProductMacro(v, v);         // always >= 0
	double    d = DotProductMacro(u, w);
	double    e = DotProductMacro(v, w);
	double    D = a * c - b * b;        // always >= 0
	double    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	double    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

	// compute the line parameters of the two closest points
	if (D < 0.00001) { // the lines are almost parallel
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b*e - c * d);
		tN = (a*e - b * d);
		if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (abs(sN) < 0.00001 ? 0.0 : sN / sD);
	tc = (abs(tN) < 0.00001 ? 0.0 : tN / tD);

	// get the difference of the two closest points
	SVector3   dP = w + (u * (float)sc) - (v * (float)tc);  // =  S1(sc) - S2(tc)

	return dP.GetLength();   // return the closest distance
}

SVector3 WorldInteractionLibrary::GetPlaneIntersection(SVector3 planePoint, SVector3 normal, SVector3 direction, SVector3 lineOrigin)
{
	direction = direction.Normalize();
	if (DotProductMacro(normal, direction) == 0) {
		return SVector3(0);
	}
	double t = (DotProductMacro(normal, planePoint) - DotProductMacro(normal, lineOrigin)) / DotProductMacro(normal, direction);
	return lineOrigin + (direction * t);
}