#include "AssetImporter.h"
#include "GLAssetIO.h"
#include "assetLibrary.h"

/** Assets */
#include "../Texture/texture.h"
#include "../Mesh/staticMesh.h"
#include "../Shader/material.h"

/** stbi */
#include "../ThirdParty/stb_image.h"

/** Assimp */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

void AssetImporter::ImportTexture(std::string textureFilePath, std::string newTextureName, std::string newFilePath)
{
	int x, y, nbrChannels;
	if (stbi_uc *data = stbi_load(textureFilePath.data(), &x, &y, &nbrChannels, 0))
	{
		std::cout << "import texture" << textureFilePath << std::endl;
		SAssetWriter writer(newFilePath);
		GLAssetIO::GenerateFileBody(writer.Get(), newTextureName, "Texture2D");
		GLAssetIO::AppendField<int*>(writer.Get(), "TextureSizeX", &x, sizeof(x));
		GLAssetIO::AppendField<int*>(writer.Get(), "TextureSizeY", &y, sizeof(y));
		GLAssetIO::AppendField<int*>(writer.Get(), "TextureChannelsCount", &nbrChannels, sizeof(nbrChannels));
		GLAssetIO::AppendField<stbi_uc*>(writer.Get(), "TextureData", data, x*y*nbrChannels);
		new Texture2D(newFilePath);
		stbi_image_free(data);
	}
}

void AssetImporter::ImportMesh(std::string meshFilePath, std::string newMeshName, std::string newFilePath)
{
	Assimp::Importer assimpImporter;
	const aiScene *scene = assimpImporter.ReadFile(meshFilePath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << assimpImporter.GetErrorString() << std::endl;
		return;
	}

	SAssetWriter writer(newFilePath);
	GLAssetIO::GenerateFileBody(writer.Get(), newMeshName, "StaticMesh");
	unsigned int sectionCount = 0;
	processNode(scene->mRootNode, scene, writer.Get(), sectionCount);
	GLAssetIO::AppendField<unsigned int*>(writer.Get(), "SectionCount", &sectionCount, sizeof(sectionCount));
	new StaticMesh(newFilePath);
}

void AssetImporter::processNode(aiNode *node, const aiScene *scene, std::ofstream* writer, unsigned int& currentSectionIndex)
{
	if (!node) return;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		if (aiMesh* mesh = scene->mMeshes[node->mMeshes[i]])
		{
			processMesh(mesh, scene, i, writer, i);
			currentSectionIndex++;
		}
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, writer, currentSectionIndex);
	}
}

void AssetImporter::processMesh(aiMesh *mesh, const aiScene *scene, unsigned int meshIndex, std::ofstream* writer, const unsigned int& SectionIndex)
{
	// data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	/** Get vertices */
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		/** Get vertice coordinate */
		{
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
		}

		/** Get or generate normals */
		{
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}
			else
			{
				vertex.Normal = glm::vec3(0.f, 0.f, 1.f);
			}
		}

		/** Get or generate texture coordinates */
		{
			if (mesh->HasTextureCoords(i))
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
			{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}
		}
		
		/** Get or generate tangent and biTangents */
		{
			if (mesh->HasTangentsAndBitangents())
			{
				// tangents
				vector.x = mesh->mTangents ? mesh->mTangents[i].x : 0.f;
				vector.y = mesh->mTangents ? mesh->mTangents[i].y : 0.f;
				vector.z = mesh->mTangents ? mesh->mTangents[i].z : 0.f;
				vertex.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents ? mesh->mBitangents[i].x : 0.f;
				vector.y = mesh->mBitangents ? mesh->mBitangents[i].y : 0.f;
				vector.z = mesh->mBitangents ? mesh->mBitangents[i].z : 0.f;
				vertex.Bitangent = vector;
			}
			else
			{
				vertex.Tangent = glm::vec3(0.f, 0.f, 1.f);
				vertex.Bitangent = glm::vec3(0.f, 0.f, 1.f);
			}
		}

		vertices.push_back(vertex);
	}

	/** Get indices */
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	
	GLAssetIO::AppendField<Vertex*>(writer, "Section" + std::to_string(SectionIndex) + "_Vertices", vertices.data(), vertices.size() * sizeof(Vertex));
	GLAssetIO::AppendField<unsigned int*>(writer, "Section" + std::to_string(SectionIndex) + "_Indices", indices.data(), indices.size() * sizeof(unsigned int));


// 	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
// 	std::vector<Texture2D*> materialTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE);
// 
// 
// 	// return a mesh object created from the extracted mesh data
// 	Material* sectionMaterial = nullptr;
// 	if (usedMaterial.size() > meshIndex)
// 	{
// 		sectionMaterial = usedMaterial[meshIndex];
// 	}
}


std::vector<Texture2D*> AssetImporter::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
	std::vector<Texture2D*> textures = {};
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		//Texture2D* texture = new Texture2D("this->directory" + '/' + std::string(str.C_Str()), true);
		std::cout << "Registered new dynamic texture " << mat->GetName().C_Str() << std::endl;
		//textures.push_back(texture);
	}
	return textures;
}