// Pierre Even et Mathieu Voyer, tous droits reservés


#include "../../Public/Mesh/ProceduralLandscapeOctreeNode.h"
#include "../../Public/Mesh/ProceduralLandscapeSection.h"
#include "../../Public/Mesh/ProceduralLandscapeComponent.h"

ProceduralLandscapeOctreeNode::ProceduralLandscapeOctreeNode(ProceduralLandscapeSection* inParentSection, const FProceduralLandscapeVector& nodeLocation, const float& halfSize, const unsigned int& lodLevel)
	: NodePosition(nodeLocation), NodeHalfSize(halfSize), NodeLODLevel(lodLevel), parentSection(inParentSection)
{
	for (auto& child : childsNodes)
	{
		child = nullptr;
	}
}


void ProceduralLandscapeOctreeNode::Update()
{

	if (!bIsMeshDataGenerated && currentMeshGenerationStatus == NodeGenerationStatus::NodeGenerationStatus_Generated)
	{
		bIsMeshDataGenerated = true;
		bCanSpawnMesh = true;
	}

	if (!bIsFoliageDataGenerated && currentFoliageGenerationStatus == NodeGenerationStatus::NodeGenerationStatus_Generated)
	{
		bIsFoliageDataGenerated = true;
	}


	RequestNodeGeneration();

	if (GetWantedLODLevel() > NodeLODLevel)
	{
		SubdivideChecked();
	}
	else
	{
		UnsubdivideChecked();
	}

	for (auto& child : childsNodes)
	{
		if (child) child->Update();
	}
}

double ProceduralLandscapeOctreeNode::GetSectionScreenSize() const
{
	ProceduralLandscapeLocationData infos;
	FProceduralLandscapeVector cameraLocation = GetParentSection()->GetParentComponent()->GetCameraLocation();
	GetParentSection()->GetParentComponent()->GetLandscapeInformationsAtLocation(FProceduralLandscapeVector2D(cameraLocation), infos);
	return ((NodeHalfSize * 2.0) / (cameraLocation - FProceduralLandscapeVector(NodePosition.X, NodePosition.Y, infos.altitude)).GetLength() * sin(GetParentSection()->GetParentComponent()->GetCameraFov()));
}

unsigned int ProceduralLandscapeOctreeNode::GetWantedLODLevel() const
{
	int outLodLevel = (int)fmax(0, fmin(NodeLODLevel + GetSectionScreenSize() / GetParentSection()->GetParentComponent()->GetNodeMaxScreenSize(), GetParentSection()->GetParentComponent()->GetMaxLODLevel()));
	return outLodLevel;
}

void ProceduralLandscapeOctreeNode::RequestNodeGeneration()
{
	TryMeshGeneration();
	if (GetParentSection()->GetParentComponent()->IsFoliageEnabled())
	{
		TryFoliageGeneration();
	}
}


void ProceduralLandscapeOctreeNode::GenerateMeshData(ProceduralLandscapeOctreeNode* usedNode)
{
	if (usedNode->currentMeshGenerationStatus == NodeGenerationStatus::NodeGenerationStatus_Generating)
	{
		usedNode->GetParentSection()->GetParentComponent()->RemoveThread();
		return;
	}
	usedNode->currentMeshGenerationStatus = NodeGenerationStatus::NodeGenerationStatus_Generating;

	int Resolution = (int)usedNode->GetParentSection()->GetParentComponent()->GetNodeMeshResolution();
	double sectionScale = usedNode->NodeHalfSize;
	double unitScale = sectionScale / Resolution * 2.f;

	bool bFlipSections = usedNode->GetParentSection()->GetParentComponent()->FlipFaces();

	FProceduralLandscapeVector2D CurrentLocation = FProceduralLandscapeVector2D(usedNode->NodePosition) - FProceduralLandscapeVector2D(sectionScale);
	std::vector<ProceduralLandscapeLocationData> pointArrayData = {};
	
	/** Procedurally generate location map */
	for (int x = -1; x <= Resolution + 1; ++x)
	{
		double posX = unitScale * x + CurrentLocation.X;

		for (int y = -1; y <= Resolution + 1; ++y)
		{
			double posY = unitScale * y + CurrentLocation.Y;
			ProceduralLandscapeLocationData localData;
			usedNode->GetParentSection()->GetParentComponent()->GetLandscapeInformationsAtLocation(FProceduralLandscapeVector2D(posX, posY), localData);
			pointArrayData.push_back(localData);	
			if (usedNode->bAbortNodeGeneration)
			{
				usedNode->GetParentSection()->GetParentComponent()->RemoveThread();
				return;
			}
		}
	}
	/** Generate normals, tangents, UVs, positions and triangles */
	for (int x = 0; x < Resolution; ++x)
	{
		for (int y = 0; y < Resolution; ++y)
		{
			for (int i = 0; i < 4; ++i)
			{
				int xOffset = i == 1 || i == 3 ? 1 : 0;
				int yOffset = i == 2 || i == 3 ? 1 : 0;

				double posX = unitScale * (x + xOffset) + CurrentLocation.X;
				double posY = unitScale * (y + yOffset) + CurrentLocation.Y;


				FProceduralLandscapeVector ptZeroLocation(0.f, 0.f, pointArrayData[(x + 1 + xOffset) * (Resolution + 3) + (y + 1 + yOffset)].altitude);
				FProceduralLandscapeVector ptXLocation(unitScale, 0, pointArrayData[(x + 2 + xOffset) * (Resolution + 3) + (y + 1 + yOffset)].altitude);
				FProceduralLandscapeVector ptYLocation(0, unitScale, pointArrayData[(x + 1 + xOffset) * (Resolution + 3) + (y + 2 + yOffset)].altitude);
				FProceduralLandscapeVector ptmXLocation(-unitScale, 0, pointArrayData[(x + xOffset)* (Resolution + 3) + (y + 1 + yOffset)].altitude);
				FProceduralLandscapeVector ptmYLocation(0, -unitScale, pointArrayData[(x + 1 + xOffset) * (Resolution + 3) + (y + yOffset)].altitude);

				FProceduralLandscapeVector normX(FProceduralLandscapeVector::CrossProduct(ptXLocation - ptZeroLocation, ptYLocation - ptXLocation));
				FProceduralLandscapeVector normY(FProceduralLandscapeVector::CrossProduct(ptYLocation - ptZeroLocation, ptmXLocation - ptXLocation));
				FProceduralLandscapeVector normmX(FProceduralLandscapeVector::CrossProduct(ptmXLocation - ptZeroLocation, ptmYLocation - ptXLocation));
				FProceduralLandscapeVector normmY(FProceduralLandscapeVector::CrossProduct(ptmYLocation - ptZeroLocation, ptXLocation - ptXLocation));

				FProceduralLandscapeVector outLocation(posX, posY, pointArrayData[(x + 1 + xOffset) * (Resolution + 3) + (y + 1 + yOffset)].altitude);
				FProceduralLandscapeVector outNormal = ((normX + normY + normmX + normmY) / 4).Normalize();
				FProceduralLandscapeVector outTangent = FProceduralLandscapeVector(FProceduralLandscapeVector::CrossProduct(outNormal.Normalize(), ptYLocation - ptZeroLocation));

				FProceduralLandscapeVector2D outUV = FProceduralLandscapeVector2D((posX - CurrentLocation.X) / (unitScale * Resolution), (posY - CurrentLocation.Y) / (unitScale * Resolution));
				FProceduralLandscapeColor outColor = pointArrayData[(x + 1 + xOffset) * (Resolution + 3) + (y + 1 + yOffset)].vertexColor;
				usedNode->NodeMeshData.AddVertice(outLocation, outNormal, outUV, outColor, outTangent);

				usedNode->NodeMeshData.AddTriangle(
					(x * Resolution + y) * 4,
					(x * Resolution + y) * 4 + (bFlipSections ? 1 : 2),
					(x * Resolution + y) * 4 + (bFlipSections ? 2 : 1)
				);

				usedNode->NodeMeshData.AddTriangle(
					(x * Resolution + y) * 4 + (bFlipSections ? 2 : 1),
					(x * Resolution + y) * 4 + (bFlipSections ? 1 : 2),
					(x * Resolution + y) * 4 + 3
				);


				if (usedNode->bAbortNodeGeneration)
				{
					usedNode->GetParentSection()->GetParentComponent()->RemoveThread();
					return;
				}
			}
		}
	}

	usedNode->PostVerticesCreation();

	usedNode->GetParentSection()->GetParentComponent()->RemoveThread();

	usedNode->currentMeshGenerationStatus = NodeGenerationStatus::NodeGenerationStatus_Generated;
}

void ProceduralLandscapeOctreeNode::GenerateFoliageData(ProceduralLandscapeOctreeNode* usedNode)
{
	if (usedNode->currentFoliageGenerationStatus == NodeGenerationStatus::NodeGenerationStatus_Generating)
	{
		usedNode->GetParentSection()->GetParentComponent()->RemoveThread();
		return;
	}
	usedNode->currentFoliageGenerationStatus = NodeGenerationStatus::NodeGenerationStatus_Generating;

	usedNode->NodeFoliageData.clear();

	int id = 0;
	for (const auto& foliage : usedNode->GetParentSection()->GetParentComponent()->GetFoliageTypes())
	{
		double unitScale = (usedNode->NodeHalfSize * 2.f) / foliage.density;

		FProceduralLandscapeNodeFoliageData newFoliage;
		newFoliage.foliageID = id;
		id++;

		if ((int)usedNode->NodeLODLevel >= foliage.minSpawnLodLevel && (int)usedNode->NodeLODLevel < foliage.maxSpawnLodLevel)
		{
			for (int x = 0; x < foliage.density; ++x)
			{
				for (int y = 0; y < foliage.density; ++y)
				{
					double posX = RandomDouble(-unitScale * foliage.randomisationRatio, unitScale * foliage.randomisationRatio) + usedNode->NodePosition.X + x * unitScale;
					double posY = RandomDouble(-unitScale * foliage.randomisationRatio, unitScale * foliage.randomisationRatio) + usedNode->NodePosition.Y + y * unitScale;
					ProceduralLandscapeLocationData data;
					usedNode->GetParentSection()->GetParentComponent()->GetLandscapeInformationsAtLocation(FProceduralLandscapeVector2D(posX, posY), data);
					if (data.altitude < foliage.minAltitude || data.altitude > foliage.maxAltitude) continue;

					double Scale = RandomDouble(foliage.minSize, foliage.maxSize);

					newFoliage.Positions.push_back(FProceduralLandscapeVector(posX, posY, data.altitude));
					newFoliage.Rotations.push_back(FProceduralLandscapeRotator(0, 0, RandomDouble(0.f, 360.f)));
					newFoliage.Scales.push_back(FProceduralLandscapeVector(Scale, Scale, Scale));
					if (usedNode->bAbortNodeGeneration)
					{
						usedNode->GetParentSection()->GetParentComponent()->RemoveThread();
						return;
					}
				}
			}
		}
		usedNode->NodeFoliageData.push_back(newFoliage);
	}

	usedNode->PostFoliageCreation();

	usedNode->GetParentSection()->GetParentComponent()->RemoveThread();

	usedNode->currentFoliageGenerationStatus = NodeGenerationStatus::NodeGenerationStatus_Generated;
}

void ProceduralLandscapeOctreeNode::SubdivideChecked()
{
	if (!bAreChildsGenerated)
	{
		bool bAreAllChildsGenerated = true;
		for (const auto& child : childsNodes)
		{
			if (child) child->bIsMeshDataGenerated;
			if (!child || !child->bIsMeshDataGenerated) bAreAllChildsGenerated = false;
		}
		if (bAreAllChildsGenerated)
		{
			bAreChildsGenerated = true;
			OnMeshRemoved();
			bCanSpawnMesh = false;
		}
	}

	if (bIsSubdivided) return;
	bIsSubdivided = true;
	ClearChilds();
	int newLodLevel = NodeLODLevel + 1;
	childsNodes[0] = GetParentSection()->GetParentComponent()->GenerateOctreeNode(GetParentSection(), NodePosition + FProceduralLandscapeVector(NodeHalfSize / 2.0, NodeHalfSize / 2.0, 0.0), NodeHalfSize / 2.0, newLodLevel);
	childsNodes[1] = GetParentSection()->GetParentComponent()->GenerateOctreeNode(GetParentSection(), NodePosition + FProceduralLandscapeVector(-NodeHalfSize / 2.0, NodeHalfSize / 2.0, 0.0), NodeHalfSize / 2.0, newLodLevel);
	childsNodes[2] = GetParentSection()->GetParentComponent()->GenerateOctreeNode(GetParentSection(), NodePosition + FProceduralLandscapeVector(NodeHalfSize / 2.0, -NodeHalfSize / 2.0, 0.0), NodeHalfSize / 2.0, newLodLevel);
	childsNodes[3] = GetParentSection()->GetParentComponent()->GenerateOctreeNode(GetParentSection(), NodePosition + FProceduralLandscapeVector(-NodeHalfSize / 2.0, -NodeHalfSize / 2.0, 0.0), NodeHalfSize / 2.0, newLodLevel);
}

void ProceduralLandscapeOctreeNode::ClearChilds()
{
	bAreChildsGenerated = false;
	for (auto& child : childsNodes)
	{
		if (child)
		{
			child->PreDestroy();
			delete child;
		}
		child = nullptr;
	}
}

void ProceduralLandscapeOctreeNode::UnsubdivideChecked()
{
	if (!bIsSubdivided) return;
	bIsSubdivided = false;
	ClearChilds();
	RequestNodeGeneration();
	bCanSpawnMesh = true;
}

void ProceduralLandscapeOctreeNode::TryFoliageGeneration()
{
	if (bIsFoliageDataGenerated)
	{
		if (bCanSpawnFoliage)
		{
			bCanSpawnFoliage = false;
			OnFoliageGenerated();
		}
	}
	else
	{
		if (bHasFoliageGenerationStarted) return;

		if (GetParentSection()->GetParentComponent()->GetCurrentThreadCount() <= 0)
		{
			if (!(foliageGenerationAsyncThread || currentFoliageGenerationStatus != NodeGenerationStatus::NodeGenerationStatus_None))
			{
				bHasFoliageGenerationStarted = true;
				currentFoliageGenerationStatus = NodeGenerationStatus::NodeGenerationStatus_WaitingForThreadStart;
				if (GetParentSection()->GetParentComponent()->EnableAsyncGeneration())
				{
					GetParentSection()->GetParentComponent()->AddThread();
					foliageGenerationAsyncThread = new std::thread(GenerateFoliageData, this);
				}
				else
				{
					GetParentSection()->GetParentComponent()->AddThread();
					GenerateFoliageData(this);
				}
			}
		}
	}
}

void ProceduralLandscapeOctreeNode::TryMeshGeneration()
{

	if (bIsMeshDataGenerated)
	{
		if (bCanSpawnMesh)
		{
			bCanSpawnMesh = false;
			OnMeshGenerated();
		}
	}
	else
	{
		if (bHasMeshGenerationStarted) return;

		if (GetParentSection()->GetParentComponent()->GetCurrentThreadCount() < GetParentSection()->GetParentComponent()->GetMaxNodeToGenerateAtOnce() || GetParentSection()->GetParentComponent()->GetMaxNodeToGenerateAtOnce() <= 0)
		{
			bHasMeshGenerationStarted = true;
			if (!(meshGenerationAsyncThread || currentMeshGenerationStatus != NodeGenerationStatus::NodeGenerationStatus_None))
			{
				currentMeshGenerationStatus = NodeGenerationStatus::NodeGenerationStatus_WaitingForThreadStart;
				if (GetParentSection()->GetParentComponent()->EnableAsyncGeneration())
				{
					GetParentSection()->GetParentComponent()->AddThread();
					meshGenerationAsyncThread = new std::thread(GenerateMeshData, this);
				}
				else
				{
					GetParentSection()->GetParentComponent()->AddThread();
					GenerateMeshData(this);
				}
			}
		}
	}
}

void ProceduralLandscapeOctreeNode::PreDestroy()
{
	bAbortNodeGeneration = true;
	if (meshGenerationAsyncThread && meshGenerationAsyncThread->joinable())
	{
		meshGenerationAsyncThread->join();
		meshGenerationAsyncThread = nullptr;
	}
	if (foliageGenerationAsyncThread && foliageGenerationAsyncThread->joinable())
	{
		foliageGenerationAsyncThread->join();
		foliageGenerationAsyncThread = nullptr;
	}
	if (!bIsSubdivided)
	{
		OnMeshRemoved();
	}
	else
	{
		ClearChilds();
	}

	if (bIsFoliageDataGenerated)
	{
		OnFoliageRemoved();
	}
}