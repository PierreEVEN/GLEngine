#include "../../Public/Mesh/ProceduralLandscapeSection.h"
#include "../../Public/Mesh/ProceduralLandscapeComponent.h"

ProceduralLandscapeSection::~ProceduralLandscapeSection()
{
	if (rootOctreeNode)
	{
		rootOctreeNode->PreDestroy();
		delete rootOctreeNode;
	}
}

void ProceduralLandscapeSection::UpdateSection()
{
	if (bIsSectionDirty || !rootOctreeNode) RegenerateSection();
	if (rootOctreeNode)
	{
		rootOctreeNode->Update();
	}
}

void ProceduralLandscapeSection::RegenerateSection()
{
	bIsSectionDirty = false;
	if (rootOctreeNode) delete rootOctreeNode;
	rootOctreeNode = parentComponent->GenerateOctreeNode(this, landscapeSectionLocation, landscapeSectionSize / 2.0, 0);
}

void ProceduralLandscapeSection::SetSectionLocation(FProceduralLandscapeVector newSectionLocation)
{
	landscapeSectionLocation = newSectionLocation;
	MarkSectionDirty();
}
