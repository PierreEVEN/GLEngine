#include "../../Public/Mesh/ProceduralLandscapeComponent.h"
#include "../../Public/Mesh/ProceduralLandscapeSection.h"
#include <iostream>
#include <utility>


bool operator< (FSectionPosition a, FSectionPosition b) { return std::make_pair(a.posX, a.posY) < std::make_pair(b.posX, b.posY); }

void ProceduralLandscapeComponent::Update()
{
	if (bIsComponentDirty) RebuildComponent();
	double baseSectionSize = GetBaseSectionSize();

	int size = (int)(GetViewDistance() / baseSectionSize);
	int offsetX = (int)(GetCameraLocation().X / baseSectionSize);
	int offsetY = (int)(GetCameraLocation().Y / baseSectionSize);


	for (int x = -size; x <= size; ++x)
	{
		for (int y = -size; y <= size; ++y)
		{
			if (sectionGrid.find(FSectionPosition(offsetX + x, offsetY + y)) == sectionGrid.end())
			{
				sectionGrid.insert({ FSectionPosition(offsetX + x, offsetY + y), GenerateSection(FProceduralLandscapeVector((x + offsetX) * baseSectionSize, (y + offsetY) * baseSectionSize, componentLocation.Z), GetBaseSectionSize()) });
			}
		}
	}

	std::vector<FSectionPosition> elementsToRemove = {};
	

	int minX = -size + offsetX;
	int minY = -size + offsetY;

	int maxX = size + offsetX;
	int maxY = size + offsetY;

	for (std::map<FSectionPosition, ProceduralLandscapeSection*>::iterator it = sectionGrid.begin(); it != sectionGrid.end(); it++)
	{
		if (it->first.posX < minX ||
			it->first.posX > maxX ||
			it->first.posY < minY ||
			it->first.posY > maxY)
		{
			delete it->second;
 			elementsToRemove.push_back(it->first);
		}
	}

	for (const auto& element : elementsToRemove)
	{
		sectionGrid.erase({ element });
	}


	for (std::map<FSectionPosition, ProceduralLandscapeSection*>::iterator it = sectionGrid.begin(); it != sectionGrid.end(); it++)
	{
		it->second->UpdateSection();
	}
}

void ProceduralLandscapeComponent::RebuildComponent()
{
	bIsComponentDirty = false;
	for (std::map<FSectionPosition, ProceduralLandscapeSection*>::iterator it = sectionGrid.begin(); it != sectionGrid.end(); it++)
	{
		delete it->second;
	}
	sectionGrid.clear();
}

ProceduralLandscapeSection* ProceduralLandscapeComponent::GenerateSection(const FProceduralLandscapeVector& sectionLocation, double inSectionSize) {
	return new ProceduralLandscapeSection(this, sectionLocation, inSectionSize);
}
