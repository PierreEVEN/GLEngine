// Pierre Even copyright all rights reserved

/************************************************************************/
/* A ProceduralLandscapeSection is a class who handle octree nodes.     */
/* It's not necessary to create derived class from this one.			*/
/************************************************************************/

#pragma once
#include "../proceduralLandscapeTypes.h"
#include "ProceduralLandscapeOctreeNode.h"

class ProceduralLandscapeOctreeNode;
class ProceduralLandscapeComponent;

class PROCEDURALLANDSCAPETOOL_API ProceduralLandscapeSection
{
protected:

	/** ProceduralLandscapeComponent who handle this section */
	ProceduralLandscapeComponent* parentComponent = nullptr;

	/** section parameters */
	FProceduralLandscapeVector landscapeSectionLocation;
	double landscapeSectionSize;

	/** Tree parent node */
	ProceduralLandscapeOctreeNode* rootOctreeNode = nullptr;

	/** call MarkSectionDirty to regenerate all nodes */
	bool bIsSectionDirty = false;

public:

	/** Constructors & destructors :  Alway create section from ProceduralLandscapeComponent */
	ProceduralLandscapeSection(ProceduralLandscapeComponent* parentComponent, FProceduralLandscapeVector inSectionLocation, double inLandscapeSectionSize) 
		: parentComponent(parentComponent) ,landscapeSectionLocation(inSectionLocation), landscapeSectionSize(inLandscapeSectionSize) {}
	virtual ~ProceduralLandscapeSection();

	/** Getter for parent component */
	ProceduralLandscapeComponent* GetParentComponent() const { return parentComponent; }

	/** Called from parent ProceduralLandscapeComponent */
	virtual void UpdateSection();

	/** Regenerate all child nodes */
	virtual void RegenerateSection();

	/** Move section location in world space */
	virtual void SetSectionLocation(FProceduralLandscapeVector newSectionLocation);

	/** Regenerate all nodes next frame */
	virtual void MarkSectionDirty() { bIsSectionDirty; }
};