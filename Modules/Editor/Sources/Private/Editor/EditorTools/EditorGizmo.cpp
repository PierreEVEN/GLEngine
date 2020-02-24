
#include <Mesh/StaticMesh.h>
#include <World/world.h>
#include <Engine/inputManager.h>
#include <MathLibrary/worldInteraction.h>
#include <Camera/camera.h>
#include <Shader/material.h>
#include <Editor/EditorTools/EditorGizmo.h>
#include <Scene/EditorScene.h>
#include <World/EditorWorld.h>
#include <Mesh/MeshData.h>

#define GIZMO_WIDTH .025f
#define GIZMO_LENGTH 2.f
#define GIZMO_ARROW_LENGTH .5f
#define GIZMO_ARROW_RADIUS .15f

void AddCylinder(std::vector<Vertex>* vArray, std::vector<unsigned int>* vTriangle, SVector3 location, SRotator rotation, float radius, float length, glm::vec4 color, unsigned int resolution = 5, bool bIncludeEnd = false, bool bDoubleSided = false)
{
	unsigned int startVertexIndex = vArray->size();
	for (unsigned int i = 0; i < resolution; ++i)
	{
		float index = (i / (float)resolution) * (float)M_PI * 2.f;
		float nextIndex = ((i + 1) / (float)resolution) * (float)M_PI * 2.f;
		
		vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4));
		vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4) + 2);
		vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4) + 1);
		
		vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4) + 1);
		vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4) + 2);
		vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4) + 3);
		
		vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(index) * radius + rotation.GetRightVector() * cos(index) * radius).ToGLVector(), color));
		vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(index) * radius + rotation.GetRightVector() * cos(index) * radius + rotation.GetForwardVector() * length).ToGLVector(), color));

		vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(nextIndex) * radius + rotation.GetRightVector() * cos(nextIndex) * radius).ToGLVector(), color));
		vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(nextIndex) * radius + rotation.GetRightVector() * cos(nextIndex) * radius + rotation.GetForwardVector() * length).ToGLVector(), color));

		if (bIncludeEnd)
		{
			vTriangle->push_back(startVertexIndex + i * 10 + 5);
			vTriangle->push_back(startVertexIndex + i * 10 + 4);
			vTriangle->push_back(startVertexIndex + i * 10 + 6);

			vTriangle->push_back(startVertexIndex + i * 10 + 7);
			vTriangle->push_back(startVertexIndex + i * 10 + 8);
			vTriangle->push_back(startVertexIndex + i * 10 + 9);

			vArray->push_back(Vertex(location.ToGLVector(), color));
			vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(index) * radius + rotation.GetRightVector() * cos(index) * radius).ToGLVector(), color));
			vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(nextIndex) * radius + rotation.GetRightVector() * cos(nextIndex) * radius).ToGLVector(), color));

			vArray->push_back(Vertex((location + rotation.GetForwardVector() * length).ToGLVector(), color));
			vArray->push_back(Vertex((location + rotation.GetForwardVector() * length + rotation.GetUpVector() * sin(index) * radius + rotation.GetRightVector() * cos(index) * radius).ToGLVector(), color));
			vArray->push_back(Vertex((location + rotation.GetForwardVector() * length + rotation.GetUpVector() * sin(nextIndex) * radius + rotation.GetRightVector() * cos(nextIndex) * radius).ToGLVector(), color));
		}
	}
	if (bDoubleSided)
	{
		unsigned int startVertexIndex = vArray->size();
		for (unsigned int i = 0; i < resolution; ++i)
		{
			float index = (i / (float)resolution) * (float)M_PI * 2.f;
			float nextIndex = ((i + 1) / (float)resolution) * (float)M_PI * 2.f;

			vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4));
			vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4) + 1);
			vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4) + 2);

			vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4) + 2);
			vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4) + 1);
			vTriangle->push_back(startVertexIndex + i * (bIncludeEnd ? 10 : 4) + 3);

			vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(index) * radius + rotation.GetRightVector() * cos(index) * radius).ToGLVector(), color));
			vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(index) * radius + rotation.GetRightVector() * cos(index) * radius + rotation.GetForwardVector() * length).ToGLVector(), color));

			vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(nextIndex) * radius + rotation.GetRightVector() * cos(nextIndex) * radius).ToGLVector(), color));
			vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(nextIndex) * radius + rotation.GetRightVector() * cos(nextIndex) * radius + rotation.GetForwardVector() * length).ToGLVector(), color));

			if (bIncludeEnd)
			{
				vTriangle->push_back(startVertexIndex + i * 10 + 4);
				vTriangle->push_back(startVertexIndex + i * 10 + 5);
				vTriangle->push_back(startVertexIndex + i * 10 + 6);

				vTriangle->push_back(startVertexIndex + i * 10 + 8);
				vTriangle->push_back(startVertexIndex + i * 10 + 7);
				vTriangle->push_back(startVertexIndex + i * 10 + 9);

				vArray->push_back(Vertex(location.ToGLVector(), color));
				vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(index) * radius + rotation.GetRightVector() * cos(index) * radius).ToGLVector(), color));
				vArray->push_back(Vertex((location + rotation.GetUpVector() * sin(nextIndex) * radius + rotation.GetRightVector() * cos(nextIndex) * radius).ToGLVector(), color));

				vArray->push_back(Vertex((location + rotation.GetForwardVector() * length).ToGLVector(), color));
				vArray->push_back(Vertex((location + rotation.GetForwardVector() * length + rotation.GetUpVector() * sin(index) * radius + rotation.GetRightVector() * cos(index) * radius).ToGLVector(), color));
				vArray->push_back(Vertex((location + rotation.GetForwardVector() * length + rotation.GetUpVector() * sin(nextIndex) * radius + rotation.GetRightVector() * cos(nextIndex) * radius).ToGLVector(), color));
			}
		}
	}
}
void AddPlane(std::vector<Vertex>* vArray, std::vector<unsigned int>* vTriangle, SVector3 location, SRotator rotation, float sizeX, float sizeY, glm::vec4 color, unsigned int resolutionX = 1, unsigned int resolutionY = 1, bool bDoubleSided = false)
{
	unsigned int startVertexIndex = vArray->size();
	for (unsigned int x = 0; x < resolutionX; ++x)
	{
		float posX = (sizeX / resolutionX) * x;
		float nextPosX = (sizeX / resolutionX) * (x + 1);
		for (unsigned int y = 0; y < resolutionY; ++y)
		{
			float posY = (sizeY / resolutionY) * y;
			float nextPosY = (sizeY / resolutionY) * (y + 1);

			vTriangle->push_back(startVertexIndex + x * resolutionX * (bDoubleSided ? 8 : 4) + y * (bDoubleSided ? 8 : 4));
			vTriangle->push_back(startVertexIndex + x * resolutionX * (bDoubleSided ? 8 : 4) + y * (bDoubleSided ? 8 : 4) + 1);
			vTriangle->push_back(startVertexIndex + x * resolutionX * (bDoubleSided ? 8 : 4) + y * (bDoubleSided ? 8 : 4) + 2);

			vTriangle->push_back(startVertexIndex + x * resolutionX * (bDoubleSided ? 8 : 4) + y * (bDoubleSided ? 8 : 4) + 1);
			vTriangle->push_back(startVertexIndex + x * resolutionX * (bDoubleSided ? 8 : 4) + y * (bDoubleSided ? 8 : 4) + 3);
			vTriangle->push_back(startVertexIndex + x * resolutionX * (bDoubleSided ? 8 : 4) + y * (bDoubleSided ? 8 : 4) + 2);

			vArray->push_back(Vertex((location + rotation.GetForwardVector() * posX + rotation.GetRightVector() * posY).ToGLVector(), color));
			vArray->push_back(Vertex((location + rotation.GetForwardVector() * nextPosX + rotation.GetRightVector() * posY).ToGLVector(), color));
			vArray->push_back(Vertex((location + rotation.GetForwardVector() * posX + rotation.GetRightVector() * nextPosY).ToGLVector(), color));
			vArray->push_back(Vertex((location + rotation.GetForwardVector() * nextPosX + rotation.GetRightVector() * nextPosY).ToGLVector(), color));

			if (bDoubleSided)
			{
				vTriangle->push_back(startVertexIndex + x * resolutionX * 8 + y * 8);
				vTriangle->push_back(startVertexIndex + x * resolutionX * 8 + y * 8 + 2);
				vTriangle->push_back(startVertexIndex + x * resolutionX * 8 + y * 8 + 1);

				vTriangle->push_back(startVertexIndex + x * resolutionX * 8 + y * 8 + 1);
				vTriangle->push_back(startVertexIndex + x * resolutionX * 8 + y * 8 + 2);
				vTriangle->push_back(startVertexIndex + x * resolutionX * 8 + y * 8 + 3);

				vArray->push_back(Vertex((location + rotation.GetForwardVector() * posX + rotation.GetRightVector() * posY).ToGLVector(), color));
				vArray->push_back(Vertex((location + rotation.GetForwardVector() * nextPosX + rotation.GetRightVector() * posY).ToGLVector(), color));
				vArray->push_back(Vertex((location + rotation.GetForwardVector() * posX + rotation.GetRightVector() * nextPosY).ToGLVector(), color));
				vArray->push_back(Vertex((location + rotation.GetForwardVector() * nextPosX + rotation.GetRightVector() * nextPosY).ToGLVector(), color));

			}
		}
	}
}
void AddSphere(std::vector<Vertex>* vArray, std::vector<unsigned int>* vTriangle, SVector3 location, float radius, glm::vec4 color, unsigned int resolutionZ = 12, unsigned int resolutionX = 6)
{
	unsigned int startVertexIndex = vArray->size();

	for (unsigned int x = 0; x < resolutionX; ++x)
	{
		float indexX = (x / (float)resolutionX) * (float)M_PI;
		float nextIndexX = ((x + 1) / (float)resolutionX) * (float)M_PI;

		for (unsigned int z = 0; z < resolutionZ; ++z)
		{
			float indexZ = (z / (float)resolutionZ) * (float)M_PI * 2.f;
			float nextIndexZ = ((z + 1) / (float)resolutionZ) * (float)M_PI * 2.f;

			vTriangle->push_back(startVertexIndex + x * resolutionZ * 4 + z * 4);
			vTriangle->push_back(startVertexIndex + x * resolutionZ * 4 + z * 4 + 2);
			vTriangle->push_back(startVertexIndex + x * resolutionZ * 4 + z * 4 + 1);

			vTriangle->push_back(startVertexIndex + x * resolutionZ * 4 + z * 4 + 1);
			vTriangle->push_back(startVertexIndex + x * resolutionZ * 4 + z * 4 + 2);
			vTriangle->push_back(startVertexIndex + x * resolutionZ * 4 + z * 4 + 3);

			float length = sin(indexX);
			float nextLength = sin(nextIndexX);

			vArray->push_back(Vertex((location + SVector3(sin(indexZ) * length, cos(indexZ) * length, cos(indexX)) * radius).ToGLVector(), color));
			vArray->push_back(Vertex((location + SVector3(sin(indexZ) * nextLength, cos(indexZ) * nextLength, cos(nextIndexX)) * radius).ToGLVector(), color));
			vArray->push_back(Vertex((location + SVector3(sin(nextIndexZ) * length, cos(nextIndexZ) * length, cos(indexX)) * radius).ToGLVector(), color));
			vArray->push_back(Vertex((location + SVector3(sin(nextIndexZ) * nextLength, cos(nextIndexZ) * nextLength, cos(nextIndexX)) * radius).ToGLVector(), color));
		}
	}
}
void AddCone(std::vector<Vertex>* vArray, std::vector<unsigned int>* vTriangle, SVector3 location, SRotator direction, float radius, float height, glm::vec4 color, unsigned int resolution = 12, bool bAddBottom = false)
{
	unsigned int startVertexIndex = vArray->size();

	for (unsigned int i = 0; i < resolution; ++i)
	{
		float index = (i / (float)resolution) * (float)M_PI * 2.f;
		float nextIndex = ((i + 1) / (float)resolution) * (float)M_PI * 2.f;

		vTriangle->push_back(startVertexIndex + i * (bAddBottom ? 6 : 3));
		vTriangle->push_back(startVertexIndex + i * (bAddBottom ? 6 : 3) + 2);
		vTriangle->push_back(startVertexIndex + i * (bAddBottom ? 6 : 3) + 1);

		vArray->push_back(Vertex((location + direction.GetForwardVector() * height).ToGLVector(), color));
		vArray->push_back(Vertex((location + SVector3(direction.GetRightVector() * sin(index)) * radius + SVector3(direction.GetUpVector() * cos(index)) * radius).ToGLVector(), color));
		vArray->push_back(Vertex((location + SVector3(direction.GetRightVector() * sin(nextIndex)) * radius + SVector3(direction.GetUpVector() * cos(nextIndex)) * radius).ToGLVector(), color));

		if (bAddBottom)
		{
			vTriangle->push_back(startVertexIndex + i * 6 + 3);
			vTriangle->push_back(startVertexIndex + i * 6 + 5);
			vTriangle->push_back(startVertexIndex + i * 6 + 4);
			vArray->push_back(Vertex((location).ToGLVector(), color));
			vArray->push_back(Vertex((location + direction.GetUpVector() * sin(index) * radius + direction.GetRightVector() * cos(index) * radius).ToGLVector(), color));
			vArray->push_back(Vertex((location + direction.GetUpVector() * sin(nextIndex) * radius + direction.GetRightVector() * cos(nextIndex) * radius).ToGLVector(), color));
		}
	}

}
void AddDisk(std::vector<Vertex>* vArray, std::vector<unsigned int>* vTriangle, SVector3 location, SRotator direction, float innerRadius, float outterRadius, glm::vec4 color, unsigned int resolution = 12, bool bDoubleSided = false)
{
	unsigned int startVertexIndex = vArray->size();

	for (unsigned int i = 0; i < resolution; ++i)
	{
		float index = (i / (float)resolution) * (float)M_PI * 2.f;
		float nextIndex = ((i + 1) / (float)resolution) * (float)M_PI * 2.f;

		vTriangle->push_back(startVertexIndex + i * 4);
		vTriangle->push_back(startVertexIndex + i * 4 + 1);
		vTriangle->push_back(startVertexIndex + i * 4 + 2);

		vTriangle->push_back(startVertexIndex + i * 4 + 1);
		vTriangle->push_back(startVertexIndex + i * 4 + 3);
		vTriangle->push_back(startVertexIndex + i * 4 + 2);

		vArray->push_back(Vertex((location + direction.GetUpVector() * innerRadius * sin(index) + direction.GetRightVector() * cos(index) * innerRadius).ToGLVector(), color));
		vArray->push_back(Vertex((location + direction.GetUpVector() * innerRadius * sin(nextIndex) + direction.GetRightVector() * cos(nextIndex) * innerRadius).ToGLVector(), color));
		vArray->push_back(Vertex((location + direction.GetUpVector() * outterRadius * sin(index) + direction.GetRightVector() * cos(index) * outterRadius).ToGLVector(), color));
		vArray->push_back(Vertex((location + direction.GetUpVector() * outterRadius * sin(nextIndex) + direction.GetRightVector() * cos(nextIndex) * outterRadius).ToGLVector(), color));
	}
	if (bDoubleSided)
	{
		startVertexIndex = vArray->size();

		for (unsigned int i = 0; i < resolution; ++i)
		{
			float index = (i / (float)resolution) * (float)M_PI * 2.f;
			float nextIndex = ((i + 1) / (float)resolution) * (float)M_PI * 2.f;

			vTriangle->push_back(startVertexIndex + i * 4);
			vTriangle->push_back(startVertexIndex + i * 4 + 2);
			vTriangle->push_back(startVertexIndex + i * 4 + 1);

			vTriangle->push_back(startVertexIndex + i * 4 + 1);
			vTriangle->push_back(startVertexIndex + i * 4 + 2);
			vTriangle->push_back(startVertexIndex + i * 4 + 3);

			vArray->push_back(Vertex((location + direction.GetUpVector() * innerRadius * sin(index) + direction.GetRightVector() * cos(index) * innerRadius).ToGLVector(), color));
			vArray->push_back(Vertex((location + direction.GetUpVector() * innerRadius * sin(nextIndex) + direction.GetRightVector() * cos(nextIndex) * innerRadius).ToGLVector(), color));
			vArray->push_back(Vertex((location + direction.GetUpVector() * outterRadius * sin(index) + direction.GetRightVector() * cos(index) * outterRadius).ToGLVector(), color));
			vArray->push_back(Vertex((location + direction.GetUpVector() * outterRadius * sin(nextIndex) + direction.GetRightVector() * cos(nextIndex) * outterRadius).ToGLVector(), color));
		}
	}
}
void AddCube(std::vector<Vertex>* vArray, std::vector<unsigned int>* vTriangle, SVector3 location, SRotator rotation, float sizeX, float sizeY, float sizeZ, glm::vec4 color)
{
	unsigned int startVertexIndex = vArray->size();

	//Bottom
	vTriangle->push_back(startVertexIndex);
	vTriangle->push_back(startVertexIndex + 2);
	vTriangle->push_back(startVertexIndex + 1);
	vTriangle->push_back(startVertexIndex + 1);
	vTriangle->push_back(startVertexIndex + 2);
	vTriangle->push_back(startVertexIndex + 3);

	//Top
	vTriangle->push_back(startVertexIndex + 4);
	vTriangle->push_back(startVertexIndex + 5);
	vTriangle->push_back(startVertexIndex + 6);
	vTriangle->push_back(startVertexIndex + 6);
	vTriangle->push_back(startVertexIndex + 5);
	vTriangle->push_back(startVertexIndex + 7);

	//Left
	vTriangle->push_back(startVertexIndex);
	vTriangle->push_back(startVertexIndex + 4);
	vTriangle->push_back(startVertexIndex + 2);
	vTriangle->push_back(startVertexIndex + 4);
	vTriangle->push_back(startVertexIndex + 6);
	vTriangle->push_back(startVertexIndex + 2);

	//Right
	vTriangle->push_back(startVertexIndex + 1);
	vTriangle->push_back(startVertexIndex + 3);
	vTriangle->push_back(startVertexIndex + 5);
	vTriangle->push_back(startVertexIndex + 3);
	vTriangle->push_back(startVertexIndex + 7);
	vTriangle->push_back(startVertexIndex + 5);

	//Front
	vTriangle->push_back(startVertexIndex);
	vTriangle->push_back(startVertexIndex + 1);
	vTriangle->push_back(startVertexIndex + 4);
	vTriangle->push_back(startVertexIndex + 1);
	vTriangle->push_back(startVertexIndex + 5);
	vTriangle->push_back(startVertexIndex + 4);

	//Bottom
	vTriangle->push_back(startVertexIndex + 3);
	vTriangle->push_back(startVertexIndex + 2);
	vTriangle->push_back(startVertexIndex + 6);
	vTriangle->push_back(startVertexIndex + 3);
	vTriangle->push_back(startVertexIndex + 6);
	vTriangle->push_back(startVertexIndex + 7);

	vArray->push_back(Vertex((location).ToGLVector(), color));//Root	0
	vArray->push_back(Vertex((location + rotation.GetForwardVector() * sizeX).ToGLVector(), color));//Root+X	 1
	vArray->push_back(Vertex((location + rotation.GetRightVector() * sizeY).ToGLVector(), color));//Root+Y	2
	vArray->push_back(Vertex((location + rotation.GetForwardVector() * sizeX + rotation.GetRightVector() * sizeY).ToGLVector(), color)); //Root+XY	3

	vArray->push_back(Vertex((location + rotation.GetUpVector() * sizeZ).ToGLVector(), color)); //Root + Z	4
	vArray->push_back(Vertex((location + rotation.GetUpVector() * sizeZ + rotation.GetForwardVector() * sizeX).ToGLVector(), color)); //Root+ZX	5
	vArray->push_back(Vertex((location + rotation.GetUpVector() * sizeZ + rotation.GetRightVector() * sizeY).ToGLVector(), color)); // Root+ZY	6
	vArray->push_back(Vertex((location + rotation.GetUpVector() * sizeZ + rotation.GetForwardVector() * sizeX + rotation.GetRightVector() * sizeY).ToGLVector(), color)); // Root+ZXY	7


}

EditorGizmo::EditorGizmo(EditorScene* inScene)
	: PrimitiveComponent(inScene)
{
	TranslationTrianglesArray = {};
	TranslationVerticesArray = {};
	AddCylinder(&TranslationVerticesArray, &TranslationTrianglesArray, SVector3(0), SRotator(0, 0, 0), GIZMO_WIDTH, GIZMO_LENGTH, glm::vec4(1, 0, 0, 1), 12, true);
	AddCylinder(&TranslationVerticesArray, &TranslationTrianglesArray, SVector3(0), SRotator(0, 0, 90), GIZMO_WIDTH, GIZMO_LENGTH, glm::vec4(0, 1, 0, 1), 12, true);
	AddCylinder(&TranslationVerticesArray, &TranslationTrianglesArray, SVector3(0), SRotator(0, -90, 0), GIZMO_WIDTH, GIZMO_LENGTH, glm::vec4(0, 0, 1, 1), 12, true);
	AddCone(&TranslationVerticesArray, &TranslationTrianglesArray, SVector3(GIZMO_LENGTH, 0, 0), SRotator(0, 0, 0), GIZMO_ARROW_RADIUS, GIZMO_ARROW_LENGTH, glm::vec4(1, 0, 0, 1), 24, true);
	AddCone(&TranslationVerticesArray, &TranslationTrianglesArray, SVector3(0, GIZMO_LENGTH, 0), SRotator(0, 0, 90), GIZMO_ARROW_RADIUS, GIZMO_ARROW_LENGTH, glm::vec4(0, 1, 0, 1), 24, true);
	AddCone(&TranslationVerticesArray, &TranslationTrianglesArray, SVector3(0, 0, GIZMO_LENGTH), SRotator(0, -90, 0), GIZMO_ARROW_RADIUS, GIZMO_ARROW_LENGTH, glm::vec4(0, 0, 1, 1), 24, true);
	AddSphere(&TranslationVerticesArray, &TranslationTrianglesArray, SVector3(0), GIZMO_ARROW_RADIUS, glm::vec4(1, 1, 1, 1), 24, 12);
	//@TODO Implement plannar translations
	//AddPlane(&TranslationVerticesArray, &TranslationTrianglesArray, SVector3(0, GIZMO_LENGTH / 5, GIZMO_LENGTH / 5), SRotator(0, -90, 0), GIZMO_LENGTH / 5, GIZMO_LENGTH / 5, glm::vec4(1, 0, 0, 1), 1, 1, true);
	//AddPlane(&TranslationVerticesArray, &TranslationTrianglesArray, SVector3(GIZMO_LENGTH / 5, 0, GIZMO_LENGTH / 5), SRotator(0, -90, -90), GIZMO_LENGTH / 5, GIZMO_LENGTH / 5, glm::vec4(0, 1, 0, 1), 1, 1, true);
	//AddPlane(&TranslationVerticesArray, &TranslationTrianglesArray, SVector3(GIZMO_LENGTH / 5, GIZMO_LENGTH / 5, 0), SRotator(0, 0, 0), GIZMO_LENGTH / 5, GIZMO_LENGTH / 5, glm::vec4(0, 0, 1, 1), 1, 1, true);


	AddSphere(&ScaleVerticesArray, &ScaleTriangleArray, SVector3(0), GIZMO_ARROW_RADIUS, glm::vec4(1, 1, 1, 1), 24, 12);
	AddCylinder(&ScaleVerticesArray, &ScaleTriangleArray, SVector3(0), SRotator(0, 0, 0), GIZMO_WIDTH, GIZMO_LENGTH, glm::vec4(1, 0, 0, 1), 12, true);
	AddCylinder(&ScaleVerticesArray, &ScaleTriangleArray, SVector3(0), SRotator(0, 0, 90), GIZMO_WIDTH, GIZMO_LENGTH, glm::vec4(0, 1, 0, 1), 12, true);
	AddCylinder(&ScaleVerticesArray, &ScaleTriangleArray, SVector3(0), SRotator(0, -90, 0), GIZMO_WIDTH, GIZMO_LENGTH, glm::vec4(0, 0, 1, 1), 12, true);
	AddCube(&ScaleVerticesArray, &ScaleTriangleArray, SVector3(GIZMO_LENGTH, -GIZMO_ARROW_RADIUS, -GIZMO_ARROW_RADIUS), SRotator(), GIZMO_ARROW_RADIUS * 2, GIZMO_ARROW_RADIUS * 2, GIZMO_ARROW_RADIUS * 2, glm::vec4(1, 0, 0, 1));
	AddCube(&ScaleVerticesArray, &ScaleTriangleArray, SVector3(-GIZMO_ARROW_RADIUS, GIZMO_LENGTH, -GIZMO_ARROW_RADIUS), SRotator(), GIZMO_ARROW_RADIUS * 2, GIZMO_ARROW_RADIUS * 2, GIZMO_ARROW_RADIUS * 2, glm::vec4(0, 1, 0, 1));
	AddCube(&ScaleVerticesArray, &ScaleTriangleArray, SVector3(-GIZMO_ARROW_RADIUS, -GIZMO_ARROW_RADIUS, GIZMO_LENGTH), SRotator(), GIZMO_ARROW_RADIUS * 2, GIZMO_ARROW_RADIUS * 2, GIZMO_ARROW_RADIUS * 2, glm::vec4(0, 0, 1, 1));

	AddDisk(&RotationVerticesArray, &RotationTriangleArray, SVector3(0), SRotator(0), GIZMO_LENGTH, GIZMO_LENGTH + .1f, glm::vec4(1, 0, 0, 1), 48, true);
	AddCylinder(&RotationVerticesArray, &RotationTriangleArray, SVector3(-.05f, 0, 0), SRotator(0), GIZMO_LENGTH + .05f, .1f, glm::vec4(1, 0, 0, 1), 48, false, true);
	AddDisk(&RotationVerticesArray, &RotationTriangleArray, SVector3(0), SRotator(0, 0, 90), GIZMO_LENGTH, GIZMO_LENGTH + .1f, glm::vec4(0, 1, 0, 1), 48, true);
	AddCylinder(&RotationVerticesArray, &RotationTriangleArray, SVector3(0, -.05f, 0), SRotator(0, 0, 90), GIZMO_LENGTH + .05f, .1f, glm::vec4(0, 1, 0, 1), 48, false, true);
	AddDisk(&RotationVerticesArray, &RotationTriangleArray, SVector3(0), SRotator(0, 90, 0), GIZMO_LENGTH, GIZMO_LENGTH + .1f, glm::vec4(0, 0, 1, 1), 48, true);
	AddCylinder(&RotationVerticesArray, &RotationTriangleArray, SVector3(0, 0, .05f), SRotator(0, 90, 0), GIZMO_LENGTH + .05f, .1f, glm::vec4(0, 0, 1, 1), 48, false, true);
	AddPlane(&RotationVerticesArray, &RotationTriangleArray, SVector3(0, -GIZMO_ARROW_LENGTH / 2, -GIZMO_ARROW_LENGTH / 2), SRotator(0, -90, 0), GIZMO_ARROW_LENGTH, GIZMO_ARROW_LENGTH, glm::vec4(1, 1, 1, 1), 1, 1, true);

	{
		// create buffers/arrays
		glGenVertexArrays(1, &TranslationVAO);
		glGenBuffers(1, &TranslationVBO);
		glGenBuffers(1, &TranlsationEBO);

		glBindVertexArray(TranslationVAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, TranslationVBO);
		glBufferData(GL_ARRAY_BUFFER, TranslationVerticesArray.size() * sizeof(Vertex), TranslationVerticesArray.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TranlsationEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, TranslationTrianglesArray.size() * sizeof(unsigned int), TranslationTrianglesArray.data(), GL_STATIC_DRAW);


		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// vertex color
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, VertexColor));
		glBindVertexArray(0);
	}
	{
		// create buffers/arrays
		glGenVertexArrays(1, &RotationVAO);
		glGenBuffers(1, &RotationVBO);
		glGenBuffers(1, &RotationEBO);

		glBindVertexArray(RotationVAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, RotationVBO);
		glBufferData(GL_ARRAY_BUFFER, RotationVerticesArray.size() * sizeof(Vertex), RotationVerticesArray.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RotationEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, RotationTriangleArray.size() * sizeof(unsigned int), RotationTriangleArray.data(), GL_STATIC_DRAW);


		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// vertex color
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, VertexColor));
		glBindVertexArray(0);
	}
	{
		// create buffers/arrays
		glGenVertexArrays(1, &ScaleVAO);
		glGenBuffers(1, &ScaleVBO);
		glGenBuffers(1, &ScaleEBO);

		glBindVertexArray(ScaleVAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, ScaleVBO);
		glBufferData(GL_ARRAY_BUFFER, ScaleVerticesArray.size() * sizeof(Vertex), ScaleVerticesArray.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ScaleEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ScaleTriangleArray.size() * sizeof(unsigned int), ScaleTriangleArray.data(), GL_STATIC_DRAW);


		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// vertex color
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, VertexColor));
		glBindVertexArray(0);
	}
	drawPriority = DrawPriority_Last;
	inScene->GetParentWorld()->GetInputManager()->AddInput(new KeyBind(inScene->GetParentWorld()->GetInputManager(), { Key(GLFW_MOUSE_BUTTON_1, true) }, ActionType_JustPress, this, & EditorGizmo::BeginDrag));
	inScene->GetParentWorld()->GetInputManager()->AddInput(new KeyBind(inScene->GetParentWorld()->GetInputManager(), { Key(GLFW_MOUSE_BUTTON_1, true) }, ActionType_JustRelease, this, &EditorGizmo::EndDrag));
	inScene->GetParentWorld()->GetInputManager()->AddInput(new KeyBind(inScene->GetParentWorld()->GetInputManager(), { Key(GLFW_KEY_Z) }, ActionType_JustPress, this, &EditorGizmo::SetTranslateMode));
	inScene->GetParentWorld()->GetInputManager()->AddInput(new KeyBind(inScene->GetParentWorld()->GetInputManager(), { Key(GLFW_KEY_R) }, ActionType_JustPress, this, &EditorGizmo::SetRotateMode));
	inScene->GetParentWorld()->GetInputManager()->AddInput(new KeyBind(inScene->GetParentWorld()->GetInputManager(), { Key(GLFW_KEY_E) }, ActionType_JustPress, this, &EditorGizmo::SetScaleMode));
	inScene->GetParentWorld()->GetInputManager()->BindOnMousePositionChanged(this, &EditorGizmo::Drag);
}

void EditorGizmo::Tick()
{
	SceneComponent::Tick();
	if (!attachedComponent) return;
	if (gizmoMode == EGizmoMode::Rotate)
	{
		SetRotation(SRotator(0, LocalRotation.GetPitch(), LocalRotation.GetYaw()));
	}
	else
	{
		if (GetRotation() != attachedComponent->GetRotation()) SetRotation(attachedComponent->GetRotation());
	}
	glm::mat4 gizmoTransform = glm::mat4(1.f);
	gizmoTransform = glm::translate(gizmoTransform, GetLocation().ToGLVector());
	gizmoTransform = gizmoTransform * glm::mat4_cast(glm::quat((float)GetRotation().w, (float)GetRotation().x, (float)GetRotation().y, (float)GetRotation().z));
	gizmoTransform = glm::scale(gizmoTransform, GetScale3D().ToGLVector());

	SVector3 traceDirection = GetScene()->PixelToWorldDirection(((EditorScene*)GetScene())->GetCursorPositionX(), ((EditorScene*)GetScene())->GetCursorPositionY()).ToGLVector();
	SVector3 xPos = GetLocation() + GetRotation().GetForwardVector() * GIZMO_LENGTH;
	SVector3 yPos = GetLocation () + GetRotation().GetRightVector() * GIZMO_LENGTH;
	SVector3 zPos = GetLocation() + GetRotation().GetUpVector() * GIZMO_LENGTH;

	SVector3 cameraPos = GetScene()->GetCamera()->GetLocation();
	SVector3 traceEndPos = cameraPos + traceDirection * 1000000.f;

	float distanceX = (float)WorldInteractionLibrary::GetSegmentDistanceFromSegment(cameraPos, traceEndPos, GetLocation(), xPos);
	float distanceY = (float)WorldInteractionLibrary::GetSegmentDistanceFromSegment(cameraPos, traceEndPos, GetLocation(), yPos);
	float distanceZ = (float)WorldInteractionLibrary::GetSegmentDistanceFromSegment(cameraPos, traceEndPos, GetLocation(), zPos);

	MaterialEditorDebuger::GetGizmoMaterial()->use();
	MaterialEditorDebuger::GetGizmoMaterial()->setMat4("model", gizmoTransform);
	MaterialEditorDebuger::GetGizmoMaterial()->setInt("gizmoMode", (int)gizmoMode);
	if (gizmoMode == EGizmoMode::Translate || gizmoMode == EGizmoMode::Scale)
	{
		MaterialEditorDebuger::GetGizmoMaterial()->setInt("direction", bIsDraging ? (int)gizmoTranslationType : (int)GetTranslationTypeFromCamera(GetScene()->GetCamera()->GetLocation(), traceDirection));
	}
	else
	{
		MaterialEditorDebuger::GetGizmoMaterial()->setInt("direction", bIsDraging ? (int)gizmoTranslationType : (int)GetRotationTypeFromCamera(GetScene()->GetCamera()->GetLocation(), traceDirection));
	}
	/** Draw vertices */
	if (gizmoMode == EGizmoMode::Translate)
	{
		glBindVertexArray(TranslationVAO);
		glDrawElements(GL_TRIANGLES, TranslationTrianglesArray.size(), GL_UNSIGNED_INT, 0);
	}
	else if (gizmoMode == EGizmoMode::Rotate)
	{
		glBindVertexArray(RotationVAO);
		glDrawElements(GL_TRIANGLES, RotationTriangleArray.size(), GL_UNSIGNED_INT, 0);
	}
	else if (gizmoMode == EGizmoMode::Scale)
	{
		glBindVertexArray(ScaleVAO);
		glDrawElements(GL_TRIANGLES, ScaleTriangleArray.size(), GL_UNSIGNED_INT, 0);
	}
	StatViewer::AddDrawcall();

	/** Set GL to defaults */
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void EditorGizmo::SetCanDrag(bool bInDrag)
{
	if (bInDrag != bIsDraging)
	{
		bIsDraging = bInDrag;
	}
}

void EditorGizmo::BeginDrag()
{
	if (bIsDraging) return;
	SVector3 traceDirection = GetScene()->PixelToWorldDirection(((EditorScene*)GetScene())->GetCursorPositionX(), ((EditorScene*)GetScene())->GetCursorPositionY());
	gizmoTranslationType = GetTranslationTypeFromCamera(GetScene()->GetCamera()->GetLocation(), traceDirection);
	gizmoTranslationType = GetTranslationTypeFromCamera(GetScene()->GetCamera()->GetLocation(), traceDirection);
	if (gizmoMode == EGizmoMode::Translate || gizmoMode == EGizmoMode::Scale)
	{
		gizmoTranslationType = GetTranslationTypeFromCamera(GetScene()->GetCamera()->GetLocation(), traceDirection);
	}
	else
	{
		gizmoTranslationType = GetRotationTypeFromCamera(GetScene()->GetCamera()->GetLocation(), traceDirection);
	}
	if (gizmoTranslationType == EGizmoTranslationType::None) return;
	SetCanDrag(true);
}

void EditorGizmo::EndDrag()
{
	SetCanDrag(false);
}

void EditorGizmo::AttachToComponent(SceneComponent* inComponent)
{
	if (inComponent)
	{
		SetLocation(inComponent->GetLocation());
		LocalScale = inComponent->GetScale3D();
		LocalRotation = inComponent->GetRotation();
		if (inComponent != attachedComponent)
		{
			SetTranslateMode();
		}
	}
	attachedComponent = inComponent;
}

void EditorGizmo::SwitchGizmoMode(const EGizmoMode& inNewMode)
{
	gizmoMode = inNewMode;
}

EGizmoTranslationType EditorGizmo::GetTranslationTypeFromCamera(SVector3 startPos, SVector3 direction)
{
	SVector3 xPos = GetLocation() + GetRotation().GetForwardVector() * GIZMO_LENGTH;
	SVector3 yPos = GetLocation() + GetRotation().GetRightVector() * GIZMO_LENGTH;
	SVector3 zPos = GetLocation() + GetRotation().GetUpVector() * GIZMO_LENGTH;

	SVector3 traceEndPos = startPos + direction * 1000000.f;

	float distanceX = (float)WorldInteractionLibrary::GetSegmentDistanceFromSegment(startPos, traceEndPos, GetLocation(), xPos);
	float distanceY = (float)WorldInteractionLibrary::GetSegmentDistanceFromSegment(startPos, traceEndPos, GetLocation(), yPos);
	float distanceZ = (float)WorldInteractionLibrary::GetSegmentDistanceFromSegment(startPos, traceEndPos, GetLocation(), zPos);

	if (distanceX < GIZMO_ARROW_RADIUS || distanceY < GIZMO_ARROW_RADIUS || distanceZ < GIZMO_ARROW_RADIUS)
	{
		if (WorldInteractionLibrary::GetPointDistanceToLine(direction, startPos, GetLocation()) < GIZMO_ARROW_RADIUS)
		{
			return EGizmoTranslationType::All;
		}
		if (distanceX < distanceY && distanceX < distanceZ)
		{
			return EGizmoTranslationType::X;
		}
		if (distanceY < distanceZ && distanceY < distanceX)
		{
			return EGizmoTranslationType::Y;
		}
		else
		{
			return EGizmoTranslationType::Z;
		}
	}
	else
	{
		return EGizmoTranslationType::None;
	}
}

EGizmoTranslationType EditorGizmo::GetRotationTypeFromCamera(SVector3 startPos, SVector3 direction)
{
	SVector3 intersecX = WorldInteractionLibrary::GetPlaneIntersection(GetLocation(), CrossProductMacro(GetRotation().GetUpVector(), GetRotation().GetRightVector()), direction, startPos);
	SVector3 intersecY = WorldInteractionLibrary::GetPlaneIntersection(GetLocation(), CrossProductMacro(GetRotation().GetForwardVector(), GetRotation().GetUpVector()), direction, startPos);
	SVector3 intersecZ = WorldInteractionLibrary::GetPlaneIntersection(GetLocation(), CrossProductMacro(GetRotation().GetForwardVector(), GetRotation().GetRightVector()), direction, startPos);

	double distanceToOriginX = (intersecX - GetLocation()).GetLength();
	double distanceToOriginY = (intersecY - GetLocation()).GetLength();
	double distanceToOriginZ = (intersecZ - GetLocation()).GetLength();

	double XDistanceToCamera = (intersecX - startPos).GetLength();
	double YDistanceToCamera = (intersecY - startPos).GetLength();
	double ZDistanceToCamera = (intersecZ - startPos).GetLength();

	bool bIsXUnderCursor = distanceToOriginX > GIZMO_LENGTH - .2f && distanceToOriginX < GIZMO_LENGTH + .2f;
	bool bIsYUnderCursor = distanceToOriginY > GIZMO_LENGTH - .2f && distanceToOriginY < GIZMO_LENGTH + .2f;
	bool bIsZUnderCursor = distanceToOriginZ > GIZMO_LENGTH - .2f && distanceToOriginZ < GIZMO_LENGTH + .2f;


	if (bIsXUnderCursor && (!bIsYUnderCursor || XDistanceToCamera < YDistanceToCamera) && (!bIsZUnderCursor || XDistanceToCamera < ZDistanceToCamera))
	{
		RotationStartDragPoint = intersecX;
		return EGizmoTranslationType::X;
	}
	else if (bIsYUnderCursor && (!bIsXUnderCursor || YDistanceToCamera < XDistanceToCamera) && (!bIsZUnderCursor || YDistanceToCamera < ZDistanceToCamera))
	{
		RotationStartDragPoint = intersecY;
		return EGizmoTranslationType::Y;
	}
	else if (bIsZUnderCursor && (!bIsXUnderCursor || ZDistanceToCamera < XDistanceToCamera) && (!bIsYUnderCursor || ZDistanceToCamera < YDistanceToCamera))
	{
		RotationStartDragPoint = intersecZ;
		return EGizmoTranslationType::Z;
	}
	else
	{
		return EGizmoTranslationType::None;
	}
}

void EditorGizmo::Drag()
{
	if (!bIsDraging) return;
	SVector3 movementVector;
	double GizmoDistance = (GetScene()->GetCamera()->GetLocation() - GetLocation()).GetLength();
	double lastXPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationX() + ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMousePositionDeltaX();
	double lastYPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationY() + ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMousePositionDeltaY();
	double xPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationX();
	double yPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationY();
	SVector3 lastDir = GetScene()->PixelToWorldDirection((float)lastXPos, (float)lastYPos);
	SVector3 newDir = GetScene()->PixelToWorldDirection((float)xPos, (float)yPos);


	movementVector = ((GetScene()->GetCamera()->GetLocation() + newDir * (float)GizmoDistance) - (GetScene()->GetCamera()->GetLocation() + lastDir * (float)GizmoDistance));

	if (gizmoMode == EGizmoMode::Translate)
	{
		if (gizmoTranslationType == EGizmoTranslationType::X)
		{
			int signe = DotProductMacro(movementVector, GetRotation().GetForwardVector()) > 0 ? 1 : -1;
			movementVector = GetRotation().GetForwardVector() * (float)movementVector.GetLength() * (float)signe;
		}
		else if (gizmoTranslationType == EGizmoTranslationType::Y)
		{
			int signe = DotProductMacro(movementVector, GetRotation().GetRightVector()) > 0 ? 1 : -1;
			movementVector = GetRotation().GetRightVector() * (float)movementVector.GetLength() * (float)signe;
		}
		else if (gizmoTranslationType == EGizmoTranslationType::Z)
		{
			int signe = DotProductMacro(movementVector, GetRotation().GetUpVector()) > 0 ? 1 : -1;
			movementVector = GetRotation().GetUpVector() * (float)movementVector.GetLength() * (float)signe;
		}
		SetLocation(GetLocation() + movementVector);
	}
	else if (gizmoMode == EGizmoMode::Rotate)
	{
		SVector3 normal;
		SVector3 directionToStartPoint = (RotationStartDragPoint - GetLocation()).Normalize();
		if (gizmoTranslationType == EGizmoTranslationType::X)
		{
			normal = SVector3(CrossProductMacro(directionToStartPoint, GetRotation().GetForwardVector()));
			int signe = DotProductMacro(movementVector, normal) > 0 ? -1 : 1;
			movementVector = SVector3(1, 0, 0) * (float)movementVector.GetLength() * (float)signe;
		}
		else if (gizmoTranslationType == EGizmoTranslationType::Y)
		{
			normal = SVector3(CrossProductMacro(directionToStartPoint, GetRotation().GetRightVector()));
			int signe = DotProductMacro(movementVector, normal) > 0 ? -1 : 1;
			movementVector = SVector3(0, 1, 0) * (float)movementVector.GetLength() * (float)signe;
		}
		else if (gizmoTranslationType == EGizmoTranslationType::Z)
		{
			normal = SVector3(CrossProductMacro(directionToStartPoint, GetRotation().GetUpVector()));
			int signe = DotProductMacro(movementVector, normal) > 0 ? -1 : 1;
			movementVector = SVector3(0, 0, 1) * (float)movementVector.GetLength() * (float)signe;
		}
		else
		{
			SVector3 direction = (GetScene()->GetCamera()->GetLocation() + newDir * (float)GizmoDistance) - GetLocation();
			movementVector = SVector3(movementVector.GetLength()) * (float)((DotProductMacro(movementVector, direction) > 0.f) ? 1.f : -1.f);
		}
		movementVector *= 10.f;
		LocalRotation = SRotator(LocalRotation.GetRoll() + movementVector.x, LocalRotation.GetPitch() + movementVector.y, LocalRotation.GetYaw() + movementVector.z);
	}
	else if (gizmoMode == EGizmoMode::Scale)
	{
		if (gizmoTranslationType == EGizmoTranslationType::X)
		{
			int signe = DotProductMacro(movementVector, GetRotation().GetForwardVector()) > 0 ? 1 : -1;
			movementVector = SVector3(1, 0, 0) * (float)movementVector.GetLength() * (float)signe;
		}
		else if (gizmoTranslationType == EGizmoTranslationType::Y)
		{
			int signe = DotProductMacro(movementVector, GetRotation().GetRightVector()) > 0 ? 1 : -1;
			movementVector = SVector3(0, 1, 0) * (float)movementVector.GetLength() * (float)signe;
		}
		else if (gizmoTranslationType == EGizmoTranslationType::Z)
		{
			int signe = DotProductMacro(movementVector, GetRotation().GetUpVector()) > 0 ? 1 : -1;
			movementVector = SVector3(0, 0, 1) * (float)movementVector.GetLength() * (float)signe;
		}
		else
		{
			SVector3 direction = (GetScene()->GetCamera()->GetLocation() + newDir * (float)GizmoDistance) - GetLocation();
			movementVector = SVector3(movementVector.GetLength()) * (float)((DotProductMacro(movementVector, direction) > 0.f) ? 1.f : -1.f);
		}
		movementVector *= .5f;
		LocalScale = movementVector + LocalScale;
	}

	if (attachedComponent)
	{
		attachedComponent->SetLocation(GetLocation());
		attachedComponent->SetRotation(LocalRotation);
		attachedComponent->SetScale3D(LocalScale);
	}
}