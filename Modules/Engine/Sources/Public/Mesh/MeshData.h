
#pragma once
#include <Scene/primitiveComponent.h>

class Material;

struct Vertex
{
	Vertex() {}
	Vertex(glm::vec3 inPosition) : Position(inPosition) {}
	Vertex(glm::vec3 inPosition, glm::vec4 inVertexColor) : Position(inPosition), VertexColor(inVertexColor) {}
	Vertex(glm::vec3 inPosition, glm::vec3 inNormal, glm::vec2 inTexCoords, glm::vec3 inTangent, glm::vec3 inBitangent, glm::vec4 inVertexColor)
		: Position(inPosition), Normal(inNormal), TexCoords(inTexCoords), Tangent(inTangent), Bitangent(inBitangent), VertexColor(inVertexColor) {}

	glm::vec3 Position = glm::vec3(0);
	glm::vec3 Normal = glm::vec3(0);
	glm::vec2 TexCoords = glm::vec2(0);
	glm::vec3 Tangent = glm::vec3(0);
	glm::vec3 Bitangent = glm::vec3(0);
	glm::vec4 VertexColor = glm::vec4(0);
};

struct MeshSectionData
{
	MeshSectionData(Vertex* newSectionVertices, unsigned int inVerticesCount, unsigned int* newSectionIndices, unsigned int inIndicesCount, Material* usedMaterial, const std::vector<Texture2D*>& usedTextures)
		: sectionVertices(newSectionVertices), verticesCount(inVerticesCount), sectionIndices(newSectionIndices), indicesCount(inIndicesCount), material(usedMaterial), textures(usedTextures)
	{
		ComputeBounds();
	}
	Vertex* sectionVertices;
	unsigned int verticesCount;
	unsigned int* sectionIndices;
	unsigned int indicesCount;
	Material* material;
	std::vector<Texture2D*> textures;
	BoxSphereBound localBounds;

	void ComputeBounds() {
		SVector3 min;
		SVector3 max;
		bool bSet = false;
		for (unsigned int verticeIndex = 0; verticeIndex < verticesCount; ++verticeIndex)
		{
			Vertex* changedVertice = &sectionVertices[verticeIndex];
			if (changedVertice->Position.x > max.x || !bSet) max.x = changedVertice->Position.x;
			if (changedVertice->Position.y > max.y || !bSet) max.y = changedVertice->Position.y;
			if (changedVertice->Position.z > max.z || !bSet) max.z = changedVertice->Position.z;

			if (changedVertice->Position.x < min.x || !bSet) min.x = changedVertice->Position.x;
			if (changedVertice->Position.y < min.y || !bSet) min.y = changedVertice->Position.y;
			if (changedVertice->Position.z < min.z || !bSet) min.z = changedVertice->Position.z;
			bSet = true;
		}
		localBounds = BoxSphereBound(min, max);
	}
};

struct MeshComponentSectionData
{
public:
	MeshComponentSectionData(const unsigned int& inVertexBuffer, const unsigned int& inVertexArray, const unsigned int& inElementBuffer, MeshSectionData inSectionData, Material* inMaterial = nullptr)
		: VBO(inVertexBuffer), VAO(inVertexArray), EBO(inElementBuffer), sectionMaterial(inMaterial) {}

private:

	unsigned int VBO, VAO, EBO;
	Material* sectionMaterial;
	MeshSectionData* sectionData;
};