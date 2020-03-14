#pragma once

#include "CorePCH.h"

class MeshComponent;
class Scene;

struct SHitResult
{
	bool bHitComponent = false;
	SVector3 posStart;
	SVector3 posEnd;
	SVector3 impactLocation;
	SVector3 impactNormal;
	MeshComponent* hitSection = nullptr;
	int instanceIndex = -1;
};


class WorldInteractionLibrary
{

public:
	static SHitResult TraceScene(Scene* traceScene, SVector3 posStart, SVector3 posEnd, bool bDrawDebugTrace = false, SVector3 traceColor = SVector3(1, 0, 0));
	static double GetPointDistanceToLine(SVector3 lineDirection, SVector3 lineOrigin, SVector3 pointLocation);
	static double GetPointDistanceToSegment(SVector3 posA, SVector3 posB, SVector3 pointLocation);
	static double GetSegmentDistanceToLine(SVector3 lineOrigin, SVector3 lineDirection, SVector3 segmentPosA, SVector3 segmentPosB);
	static double GetSegmentDistanceFromSegment(SVector3 SegAPosA, SVector3 SegAPosB, SVector3 SegBPosA, SVector3 SegBPosB);
	static SVector3 GetPlaneIntersection(SVector3 planePoint, SVector3 normal, SVector3 direction, SVector3 lineOrigin);
};