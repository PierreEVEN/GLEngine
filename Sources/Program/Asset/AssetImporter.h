#pragma once

#include <string>
#include <vector>

struct StaticMeshSection;
class Texture2D;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;
class Texture2D;

class AssetImporter
{
public:

	/************************************************************************/
	/* Texture importer                                                     */
	/************************************************************************/

	static void ImportTexture2D(std::string textureFilePath, std::string newTextureName, std::string newFilePath);

	/************************************************************************/
	/* TextureCube importer                                                     */
	/************************************************************************/

	static void ImportTextureCube(std::string textures[6], std::string newTextureName, std::string newFilePath);


	/************************************************************************/
	/* MeshImporter                                                         */
	/************************************************************************/

	static void ImportMesh(std::string meshFilePath, std::string newMeshName, std::string newFilePath, bool importMaterials);
	static void processNode(aiNode *node, const aiScene *scene, std::ofstream* writer, unsigned int& currentSectionIndex, bool importMaterials, std::string meshName);
	static void processMesh(aiMesh *mesh, const aiScene *scene, unsigned int meshIndex, std::ofstream* writer, bool importMaterials, std::string meshName);
	static std::string loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string meshName, int meshIndex);

	/************************************************************************/
	/* MaterialImporter                                                     */
	/************************************************************************/

	static void ImportShader(std::string vertexShaderFilePath, std::string fragmentShaderFilePath, std::string newShaderName, std::string newFilePath, std::vector<std::string> linkedTextures);
};