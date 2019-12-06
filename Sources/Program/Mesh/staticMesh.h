

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
public:

	std::string dataPath;
	std::vector<StaticMeshSection> meshSections;
	std::vector<Material*> usedMaterial;

	StaticMesh(std::string dataAssetPath);

	void LoadData(std::string path);

};