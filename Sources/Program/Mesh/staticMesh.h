#pragma once

#include <glm/gtx/matrix_decompose.hpp>
#include "../Asset/asset.h"
#include <vector>
#include "../Physic/primitiveComponent.h"

class Material;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;
class Texture2D;
class Scene;
struct BoxSphereBound;

struct OldVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Vertex
{
	Vertex() {}
	Vertex(glm::vec3 inPosition) : Position(inPosition) {}
	Vertex(glm::vec3 inPosition, glm::vec4 inVertexColor) : Position(inPosition), VertexColor(inVertexColor) {}
	Vertex(OldVertex inOldVertex) : Position(inOldVertex.Position), Normal(inOldVertex.Normal), TexCoords(inOldVertex.TexCoords), Tangent(inOldVertex.Tangent), Bitangent(inOldVertex.Bitangent) {}
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	glm::vec4 VertexColor;
};

struct StaticMeshSection
{
	StaticMeshSection(const std::vector<Vertex>& newSectionVertices, const std::vector<unsigned int>& newSectionIndices, Material* usedMaterial, const std::vector<Texture2D*>& usedTextures)
	{
		sectionVertices = newSectionVertices;
		sectionIndices = newSectionIndices;
		material = usedMaterial;
		textures = usedTextures;
		ComputeBounds();
	}
	std::vector<Vertex> sectionVertices;
	std::vector<unsigned int> sectionIndices;
	Material* material;
	std::vector<Texture2D*> textures;
	BoxSphereBound bounds;
	void ComputeBounds();

};

class StaticMesh : public Asset
{
private:

	std::vector<Material*> usedMaterial;
	std::vector<StaticMeshSection*> meshSections;

public:

	StaticMesh(std::string dataAssetPath);

	std::vector<StaticMeshSection*> GetSections();
	std::vector<Material*> GetMaterials();

	BoxSphereBound bounds;
	void ComputeBounds();
protected:

	virtual void ImportData() override;
	virtual void OnPropertiesLoaded() override;

public:

	virtual void UpdateMaterials();

	virtual void BuildThumbnail() override;
	virtual ImColor GetAssetColor() { return ImColor(0.7f, 0.6f, 0.4f, 1.f); }
	virtual void OnAssetClicked() override;
	virtual void SaveAsset() override;
};