

#include <glm/gtx/matrix_decompose.hpp>
#include "../Asset/asset.h"
#include <vector>

class Material;
struct aiNode;
struct aiScene;
struct aiMesh;

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
	StaticMeshSection(std::vector<Vertex> newSectionVertices, std::vector<unsigned int> newSectionIndices, Material* usedMaterial)
	{
		sectionVertices = newSectionVertices;
		sectionIndices = newSectionIndices;
		material = usedMaterial;
	}
	std::vector<Vertex> sectionVertices;
	std::vector<unsigned int> sectionIndices;
	Material* material;
};

class StaticMesh : public Asset
{
public:

	std::string dataPath;
	std::vector<StaticMeshSection> meshSections;
	std::vector<Material*> usedMaterial;

	StaticMesh(std::string dataAssetPath) : Asset(dataAssetPath) {}
	StaticMesh(std::string newDataPath, std::vector<Material*> newUsedMaterials);

	void LoadData(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	StaticMeshSection processMesh(aiMesh *mesh, const aiScene *scene, unsigned int meshIndex);

	virtual void Parse(const Document& data) override;
};