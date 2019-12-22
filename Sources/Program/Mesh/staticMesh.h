#pragma once

#include <glm/gtx/matrix_decompose.hpp>
#include "../Asset/asset.h"
#include <vector>

class Material;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;
class Texture2D;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct StaticMeshSection
{
	StaticMeshSection(std::vector<Vertex> newSectionVertices, std::vector<unsigned int> newSectionIndices, Material* usedMaterial, std::vector<Texture2D*> usedTextures)
	{
		sectionVertices = newSectionVertices;
		sectionIndices = newSectionIndices;
		material = usedMaterial;
		textures = usedTextures;
	}
	std::vector<Vertex> sectionVertices;
	std::vector<unsigned int> sectionIndices;
	Material* material;
	std::vector<Texture2D*> textures;
};

class StaticMesh : public Asset
{
private:

	std::vector<Material*> usedMaterial;

public:

	std::vector<StaticMeshSection> meshSections;

	StaticMesh(std::string dataAssetPath);

	std::vector<StaticMeshSection>* GetSections()
	{
		LoadData();
		return &meshSections;
	}

	std::vector<Material*> GetMaterials()
	{
		LoadData();
		return usedMaterial;
	}

	void LoadMesh(std::string path);

protected:

	virtual void ImportData() override;

public:

	virtual ImColor GetAssetColor() { return ImColor(0.7f, 0.6f, 0.4f, 1.f); }
	virtual void OnAssetClicked() override;

};