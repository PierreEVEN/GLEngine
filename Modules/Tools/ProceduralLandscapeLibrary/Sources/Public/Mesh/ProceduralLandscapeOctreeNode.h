// Pierre Even et Mathieu Voyer, tous droits reservés

#pragma once
#include <vector>
#include <thread>
#include "../proceduralLandscapeTypes.h"

class ProceduralLandscapeSection;
class ProceduralLandscapeComponent;

class PROCEDURALLANDSCAPETOOL_API ProceduralLandscapeOctreeNode
{
protected:

	/** Node position in worldSpace */
	FProceduralLandscapeVector NodePosition;
	/** Half size of this node in world space */
	float NodeHalfSize;
	/** Current subdivision level */
	unsigned int NodeLODLevel;

	/** generated data */
	FProceduralLandscapeNodeMeshData NodeMeshData;
	std::vector<FProceduralLandscapeNodeFoliageData> NodeFoliageData;

	/** Async generation threads */
	std::thread* meshGenerationAsyncThread = nullptr;
	std::thread* foliageGenerationAsyncThread = nullptr;

	/** Async generation state */
	NodeGenerationStatus currentMeshGenerationStatus = NodeGenerationStatus::NodeGenerationStatus_None;
	NodeGenerationStatus currentFoliageGenerationStatus = NodeGenerationStatus::NodeGenerationStatus_None;

	/** Node state */
	bool bIsFoliageDataGenerated = false;
	bool bIsMeshDataGenerated = false;
	bool bIsSubdivided = false;
	bool bAreChildsGenerated = false;
	bool bHasMeshGenerationStarted = false;
	bool bHasFoliageGenerationStarted = false;
	bool bCanSpawnMesh = true;
	bool bCanSpawnFoliage = true;
	bool bAbortNodeGeneration = false;

	/** section who handle this node hierarchye */
	ProceduralLandscapeSection* parentSection;
	/** Child node table (4 node per subdivision) */
	ProceduralLandscapeOctreeNode* childsNodes[4];

private:

	/** Build mesh data */
	static void GenerateMeshData(ProceduralLandscapeOctreeNode* usedNode);
	/** Build foliage data */
	static void GenerateFoliageData(ProceduralLandscapeOctreeNode* usedNode);

	/** Create child sections if all child section datas are build and childs aren't already generated */
	void SubdivideChecked();
	/** Destroy child sections */
	void ClearChilds();
	/** Delete child sections, then generate mesh */
	void UnsubdivideChecked();

	void TryMeshGeneration();
	void TryFoliageGeneration();

	/** Generate random float in range */
	static double RandomDouble(double a, double b) {
		double random = ((double)rand()) / (double)RAND_MAX;
		double diff = b - a;
		double r = random * diff;
		return a + r;
	}

public:

	/** Destroy childs and wait for full async generation termination */
	void PreDestroy();

	/** Constructors & destructors */
	ProceduralLandscapeOctreeNode(ProceduralLandscapeSection* inParentSection, const FProceduralLandscapeVector& nodeLocation, const float& halfSize, const unsigned int& lodLevel);
	virtual ~ProceduralLandscapeOctreeNode() {}

	/** Cycle node state. Called every landscape update from parent */
	virtual void Update();
	/** Compute section screen size from FOV */
	virtual double GetSectionScreenSize() const;	
	/** Compute relevant lod level from screen size and camera location */
	virtual unsigned int GetWantedLODLevel() const;
	/** Getter for parent section */
	ProceduralLandscapeSection* GetParentSection() const { return parentSection; }

protected:

	/** Start mesh generation procedure */
	void RequestNodeGeneration();
	/** {optional} Called after vertice creation on async thread */
	virtual void PostVerticesCreation() {}
	/** {optional} Called after foliage creation on async thread */
	virtual void PostFoliageCreation() {}
	/** Called when all the vertices are created on game Thread */
	virtual void OnMeshGenerated() = 0;	
	/** Called when all the foliage points are created on game Thread */
	virtual void OnFoliageGenerated() = 0;
	/** Called when a mesh section need to be removed */
	virtual void OnMeshRemoved() = 0;
	/** Called when a foliage section need to be removed */
	virtual void OnFoliageRemoved() = 0;
};