#pragma once

#ifndef PROCEDURALLANDSCAPETOOL_API //UNREAL ENGINE IMPLEMENTATION
#define PROCEDURALLANDSCAPETOOL_API
#endif // !PROCEDURALLANDSCAPETOOL_API


#include <math.h>  
#include <vector>

enum class NodeGenerationStatus
{
	NodeGenerationStatus_None,
	NodeGenerationStatus_WaitingForThreadStart,
	NodeGenerationStatus_Generating,
	NodeGenerationStatus_Generated
};

struct FProceduralLandscapeColor
{
	char r;
	char g;
	char b;
	char a;
};

struct ProceduralLandscapeLocationData
{
	double altitude;
	FProceduralLandscapeColor vertexColor;
};

struct FProceduralLandscapeRotator
{
	FProceduralLandscapeRotator() {}

	FProceduralLandscapeRotator(double posX, const double& posY, const double& posZ)
		: X(posX), Y(posY), Z(posZ) {}

	double X;
	double Y;
	double Z;
};

struct FSectionPosition
{
public:
	FSectionPosition() {}
	FSectionPosition(int inPosX, int inPosY) : posX(inPosX), posY(inPosY) {}
	int posX;
	int posY;
};

struct FProceduralLandscapeVector
{
	FProceduralLandscapeVector() {}

	FProceduralLandscapeVector(double posX, const double& posY, const double& posZ)
		: X(posX), Y(posY), Z(posZ) {}

	double X;
	double Y;
	double Z;

	FProceduralLandscapeVector operator-(const FProceduralLandscapeVector& other)
	{
		return FProceduralLandscapeVector(X - other.X, Y - other.Y, Z - other.Z);
	}

	FProceduralLandscapeVector operator+(const FProceduralLandscapeVector& other)
	{
		return FProceduralLandscapeVector(X + other.X, Y + other.Y, Z + other.Z);
	}

	FProceduralLandscapeVector operator/(const double& other)
	{
		return FProceduralLandscapeVector(X / other, Y / other, Z / other);
	}

	FProceduralLandscapeVector Normalize() const
	{
		return FProceduralLandscapeVector(X, Y, Z) / (double)GetLength();
	}

	double DistanceTo(const FProceduralLandscapeVector& other) const
	{
		return FProceduralLandscapeVector(X - other.X, Y - other.Y, Z - other.Z).GetLength();
	}

	double GetLength() const
	{
		return sqrt(X * X + Y * Y + Z * Z);
	}

	static FProceduralLandscapeVector CrossProduct(const FProceduralLandscapeVector& A, const FProceduralLandscapeVector& B)
	{
		return FProceduralLandscapeVector
		(
			A.Y * B.Z - A.Z * B.Y,
			A.Z * B.X - A.X * B.Z,
			A.X * B.Y - A.Y * B.X
		);
	}
};

struct FProceduralLandscapeVector2D
{
	FProceduralLandscapeVector2D() {}
	FProceduralLandscapeVector2D(const double& posX, const double& posY)
		: X(posX), Y(posY) {}

	FProceduralLandscapeVector2D(const double& pos)
		: X(pos), Y(pos) {}

	FProceduralLandscapeVector2D(const FProceduralLandscapeVector& inVec3D)
		: X(inVec3D.X), Y(inVec3D.Y) {}

	double X;
	double Y;

	FProceduralLandscapeVector2D operator-(const FProceduralLandscapeVector2D& other)
	{
		return FProceduralLandscapeVector2D(X - other.X, Y - other.Y);
	}

	FProceduralLandscapeVector2D operator+(const FProceduralLandscapeVector2D& other)
	{
		return FProceduralLandscapeVector2D(X + other.X, Y + other.Y);
	}

	FProceduralLandscapeVector To3DVector() const { return FProceduralLandscapeVector(X, Y, 0.0); }
};


struct FProceduralLandscapeNodeFoliageData
{
public:

	FProceduralLandscapeNodeFoliageData() {}

	int foliageID;

	std::vector<FProceduralLandscapeVector> Positions;
	std::vector<FProceduralLandscapeRotator> Rotations;
	std::vector<FProceduralLandscapeVector> Scales;
};

struct FProceduralLandscapeNodeMeshData
{
public:
	std::vector<FProceduralLandscapeVector> Vertices;
	std::vector<unsigned int> Indices;
	std::vector<FProceduralLandscapeVector> Normals;
	std::vector<FProceduralLandscapeVector2D> UVs;
	std::vector<FProceduralLandscapeColor> VertexColors;
	std::vector<FProceduralLandscapeVector> Tangents;

	FProceduralLandscapeNodeMeshData() {}

	void AddVertice(const FProceduralLandscapeVector& location, const FProceduralLandscapeVector& normal, const FProceduralLandscapeVector2D& uvCoordinates, const FProceduralLandscapeColor& vertexColor, const FProceduralLandscapeVector& tangent) {
		Vertices.push_back(location);
		Normals.push_back(normal);
		UVs.push_back(uvCoordinates);
		VertexColors.push_back(vertexColor);
		Tangents.push_back(tangent);
	}

	void AddTriangle(const unsigned int& pos1, const unsigned int& pos2, const unsigned int& pos3) {
		Indices.push_back(pos1);
		Indices.push_back(pos2);
		Indices.push_back(pos3);
	}

	void Clear() {
		Vertices.clear();
		Indices.clear();
		Normals.clear();
		UVs.clear();
		VertexColors.clear();
		Tangents.clear();
	}

	FProceduralLandscapeVector* GetVerticesData() { return Vertices.data(); }
	unsigned int* GetIndicesData() { return Indices.data(); }
	FProceduralLandscapeVector* GetNormalsData() { return Normals.data(); }
	FProceduralLandscapeVector2D* GetUVsData() { return UVs.data(); }
	FProceduralLandscapeColor* GetVertexColorsData() { return VertexColors.data(); }
	FProceduralLandscapeVector* GetTangentsData() { return Tangents.data(); }

	unsigned int GetVerticesCount() const { return Vertices.size(); }
	unsigned int GetIndicesCount() const { return Indices.size(); }
	unsigned int GetNormalsCount() const { return Normals.size(); }
	unsigned int GetUVsCount() const { return UVs.size(); }
	unsigned int GetVertexColorsCount() const { return VertexColors.size(); }
	unsigned int GetTangentsCount() const { return Tangents.size(); }
};

struct FProceduralFoliageType
{
public:

	FProceduralFoliageType() {}

	int density = 20;
	double randomisationRatio = 1.f;

	int minSpawnLodLevel = 2;
	int maxSpawnLodLevel = 5;

	double minAltitude = 20.f;
	double maxAltitude = 2000.f;

	double minNormalAngle = 0.f;
	double maxNormalAngle = 0.5f;

	double minSize = 0.8f;
	double maxSize = 1.2f;

	bool bAlignOnSurface = false;
};