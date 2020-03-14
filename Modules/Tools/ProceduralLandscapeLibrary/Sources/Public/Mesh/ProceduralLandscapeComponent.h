#pragma once
#include "../proceduralLandscapeTypes.h"
#include <map>

class ProceduralLandscapeSection;
class ProceduralLandscapeOctreeNode;


class PROCEDURALLANDSCAPETOOL_API ProceduralLandscapeComponent
{

private:

	/** section 2D grid in world */
	std::map<FSectionPosition, ProceduralLandscapeSection*> sectionGrid = {};	
	/** Component world location */
	FProceduralLandscapeVector componentLocation;
	/** dirty component are rebuild next update */
	bool bIsComponentDirty;
	/** Landscape foliage parameters */
	std::vector<FProceduralFoliageType> foliageTypes;
	/** Current component thread count */
	int generatingThreadsCount = 0;


public:

	/** Constructors & destructors */
	ProceduralLandscapeComponent() : componentLocation(FProceduralLandscapeVector(0, 0, 0)), bIsComponentDirty(true) {}
	ProceduralLandscapeComponent(FProceduralLandscapeVector inLocation) : componentLocation(inLocation), bIsComponentDirty(true) {}
	virtual ~ProceduralLandscapeComponent() {}

	/** Call Update each time you need to update your landscape. If you don't want to update your landscape each frame, disable async generation */
	virtual void Update();

	/** regenerate component next update */
	void MarkComponentDirty() { bIsComponentDirty = true; }

	/** Regenerate all subSections */
	virtual void RebuildComponent();

	void AddThread() { generatingThreadsCount++; }
	void RemoveThread() { generatingThreadsCount--; }
	int GetCurrentThreadCount() { return generatingThreadsCount; }

	/** Implement to use you own octree node class */
	virtual ProceduralLandscapeOctreeNode* GenerateOctreeNode(ProceduralLandscapeSection* parentSection, const FProceduralLandscapeVector& nodeLocation, const double& halfSize, const unsigned int& lodLevel) = 0;
	/** Implement to use you own sectioon class */
	virtual ProceduralLandscapeSection* GenerateSection(const FProceduralLandscapeVector& sectionLocation, double inSectionSize);

	/************************************************************************/
	/* Parameters implementation                                            */
	/************************************************************************/

	//Component parameters

	/** Get view distance (number of seen sections) */
	virtual double GetViewDistance() const = 0;
	/** Get base section size */
	virtual double GetBaseSectionSize() const = 0;
	/** Get camera world location */
	virtual FProceduralLandscapeVector GetCameraLocation() const = 0;
	/** Get camera Field of view (used to compute node screen size) */
	virtual double GetCameraFov() const = 0;

	/** Update and get foliage setup */
	std::vector<FProceduralFoliageType> GetFoliageTypes() const { return foliageTypes; }
	void SetFoliageTypes(const std::vector<FProceduralFoliageType>& inNewFoliageTypes) { foliageTypes = inNewFoliageTypes; }

	//Node parameters

	/** Vertices² per node */
	virtual unsigned int GetNodeMeshResolution() const = 0;
	/** Does we generate foliage */
	virtual bool IsFoliageEnabled() const { return true; }
	/** Lod levels per section */
	virtual unsigned int GetMaxLODLevel() const = 0;
	/** call Update() each frame if you enabled async generation */
	virtual bool EnableAsyncGeneration() const = 0;
	/** Max node to generate at once asynchronously */
	virtual int GetMaxNodeToGenerateAtOnce() const = 0;
	/** Implement you own generation rules here */
	virtual void GetLandscapeInformationsAtLocation(const FProceduralLandscapeVector2D& location, ProceduralLandscapeLocationData& outData) const = 0;
	/** Flip triangle faces */
	virtual bool FlipFaces() const = 0;
	/** Node max size on screen */
	virtual float GetNodeMaxScreenSize() const = 0;
};