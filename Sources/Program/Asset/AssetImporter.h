#ifndef ASSETIMPORTER_H
#define ASSETIMPORTER_H

#include <string>
#include <vector>

struct StaticMeshSection;
class Texture2D;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

class AssetImporter
{
public:

	/************************************************************************/
	/* Texture importer                                                     */
	/************************************************************************/

	static void ImportTexture(std::string textureFilePath, std::string newTextureName, std::string newFilePath);

	/************************************************************************/
	/* MeshImporter                                                         */
	/************************************************************************/

	static void ImportMesh(std::string meshFilePath, std::string newMeshName, std::string newFilePath);
	static void processNode(aiNode *node, const aiScene *scene, std::ofstream* writer, unsigned int& currentSectionIndex);
	static void processMesh(aiMesh *mesh, const aiScene *scene, unsigned int meshIndex, std::ofstream* writer, const unsigned int& SectionIndex);
	static std::vector<Texture2D*> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

	/************************************************************************/
	/* MaterialImporter                                                     */
	/************************************************************************/


};
#endif