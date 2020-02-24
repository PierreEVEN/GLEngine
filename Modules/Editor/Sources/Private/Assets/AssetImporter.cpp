
/** stbi */
#include <stb_image.h>

/** Assimp */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include "Asset/GLAssetIO.h"
#include <assert.h>
#include <Texture/texture.h>
#include <Mesh/StaticMesh.h>
#include <Shader/shaderLoader.h>
#include <Asset/assetLibrary.h>
#include <Shader/material.h>
#include "EngineLog/engineLog.h"
#include <Assets/AssetImporter.h>
#include <Mesh/MeshData.h>

void AssetImporter::ImportTexture2D(std::string textureFilePath, std::string newTextureName, std::string newFilePath)
{
	int x, y, nbrChannels;
	if (stbi_uc *data = stbi_load(textureFilePath.data(), &x, &y, &nbrChannels, 0))
	{
		GFullLog(LogVerbosity::Display, "AssetImporter", "Importing texture from " + textureFilePath);
		newFilePath += ".glAsset";
		SAssetWriter writer(newFilePath);
		GLAssetIO::GenerateFileBody(writer.Get(), newTextureName, "Texture2D");
		GLAssetIO::AppendField<int*>(writer.Get(), "TextureSizeX", &x, sizeof(x));
		GLAssetIO::AppendField<int*>(writer.Get(), "TextureSizeY", &y, sizeof(y));
		GLAssetIO::AppendField<int*>(writer.Get(), "TextureChannelsCount", &nbrChannels, sizeof(nbrChannels));
		GLAssetIO::AppendField<stbi_uc*>(writer.Get(), "TextureData", data, x*y*nbrChannels);
		writer.ForceCloseFile();
		new Texture2D(newFilePath);
		stbi_image_free(data);
	}
	else
	{
		GFullLog(LogVerbosity::Error, "AssetImporter", "failed to find texture " + textureFilePath);
	}
}

void AssetImporter::ImportTextureCube(std::string textures[6], std::string newTextureName, std::string newFilePath)
{
	newFilePath += ".glAsset";
	SAssetWriter writer(newFilePath);
	assert(writer.IsValid());
	GLAssetIO::GenerateFileBody(writer.Get(), newTextureName, "TextureCube");
	for (unsigned int i = 0; i < 6; ++i)
	{
		int x, y, nbrChannels;
		if (stbi_uc* data = stbi_load(textures[i].data(), &x, &y, &nbrChannels, 0))
		{
			GFullLog(LogVerbosity::Display, "AssetImporter", "Importing texture from " + textures[i] + " (index " + std::to_string(i) + ")");
			assert(writer.IsValid());
			GLAssetIO::AppendField<int*>(writer.Get(), "TextureSizeX_" + std::to_string(i), &x, sizeof(x));
			GLAssetIO::AppendField<int*>(writer.Get(), "TextureSizeY_" + std::to_string(i), &y, sizeof(y));
			GLAssetIO::AppendField<int*>(writer.Get(), "TextureChannelsCount_" + std::to_string(i), &nbrChannels, sizeof(nbrChannels));
			GLAssetIO::AppendField<stbi_uc*>(writer.Get(), "TextureData_" + std::to_string(i), data, x * y * nbrChannels);
			stbi_image_free(data);
		}
		else
		{
			GFullLog(LogVerbosity::Error, "AssetImporter", "failed to find texture " + textures[i]);
		}
	}
	writer.ForceCloseFile();
	new TextureCube(newFilePath);
}

void AssetImporter::ImportMesh(std::string meshFilePath, std::string newMeshName, std::string newFilePath, bool importMaterials)
{
	Assimp::Importer assimpImporter;
	const aiScene *scene = assimpImporter.ReadFile(meshFilePath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		GFullLog(LogVerbosity::Error, "AssetImporter", "ERROR::ASSIMP::" + std::string(assimpImporter.GetErrorString()));
		return;
	}

	newFilePath += ".glAsset";

	SAssetWriter writer(newFilePath);
	GLAssetIO::GenerateFileBody(writer.Get(), newMeshName, "StaticMesh");
	unsigned int sectionCount = 0;
	processNode(scene->mRootNode, scene, writer.Get(), sectionCount, importMaterials, newMeshName);
	GLAssetIO::AppendField<unsigned int*>(writer.Get(), "SectionCount", &sectionCount, sizeof(sectionCount));
	writer.ForceCloseFile();
	new AStaticMesh(newFilePath);
}

void AssetImporter::processNode(aiNode *node, const aiScene *scene, std::ofstream* writer, unsigned int& currentSectionIndex, bool importMaterials, std::string meshName)
{
	if (!node) return;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		if (aiMesh* mesh = scene->mMeshes[node->mMeshes[i]])
		{
			processMesh(mesh, scene, currentSectionIndex, writer, importMaterials, meshName);
			currentSectionIndex++;
		}
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, writer, currentSectionIndex, importMaterials, meshName);
	}
}

void AssetImporter::processMesh(aiMesh *mesh, const aiScene *scene, unsigned int meshIndex, std::ofstream* writer, bool importMaterials, std::string meshName)
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
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
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

		/** Get or generate vertex colors */
		{
			if (mesh->HasVertexColors(i))
			{
				// tangents
				glm::vec4 color;
				color.r = mesh->mColors ? mesh->mColors[i]->r : 0.f;
				color.g = mesh->mColors ? mesh->mColors[i]->g : 0.f;
				color.b = mesh->mColors ? mesh->mColors[i]->b : 0.f;
				color.a = mesh->mColors ? mesh->mColors[i]->a : 0.f;
				vertex.VertexColor = color;
			}
			else
			{
				vertex.VertexColor = glm::vec4(0.f, 0.f, 0.f, 1.f);
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
	
	GLAssetIO::AppendField<Vertex*>(writer, "Section" + std::to_string((int)meshIndex) + "_Vertices", vertices.data(), vertices.size() * sizeof(Vertex));
	GLAssetIO::AppendField<unsigned int*>(writer, "Section" + std::to_string((int)meshIndex) + "_Indices", indices.data(), indices.size() * sizeof(unsigned int));


	/** Get materials */
	if (importMaterials)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::string newMaterial = loadMaterialTextures(material, aiTextureType_DIFFUSE, meshName, (int)meshIndex);
		GLAssetIO::AppendField<char*>(writer, "Material_" + std::to_string(meshIndex), (char*)newMaterial.data(), newMaterial.size() + 1);
	}
}


std::string AssetImporter::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string meshName, int meshIndex)
{
	std::vector<std::string> textures = {};
	if (mat)
	{
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			std::string newTextureName = AssetLibrary::GenerateNonExistingAssetName("Texture_" + meshName + std::to_string(meshIndex));
			textures.push_back(newTextureName);
			AssetImporter::ImportTexture2D(str.C_Str(), newTextureName, "./Sources/Assets/Textures/" + newTextureName);
		}
	}
	std::string newMaterialName = AssetLibrary::GenerateNonExistingAssetName("Material_" + meshName + std::to_string(meshIndex));
	AssetImporter::ImportShader("Sources/Shaders/Campus/campusVertexShader.vs", "Sources/Shaders/Campus/campusFragmentShader.fs", newMaterialName, "./Sources/Assets/Materials/" + newMaterialName, textures);
	return newMaterialName;
}

void AssetImporter::ImportShader(std::string vertexShaderFilePath, std::string fragmentShaderFilePath, std::string newShaderName, std::string newFilePath, std::vector<std::string> linkedTextures)
{
	ShaderLoader* compiler = new ShaderLoader(vertexShaderFilePath.data(), fragmentShaderFilePath.data());

	newFilePath += ".glAsset";
	SAssetWriter writer(newFilePath);

	GLAssetIO::GenerateFileBody(writer.Get(), newShaderName, "Material");
	GLAssetIO::AppendField<char*>(writer.Get(), "VertexShaderFilePath", (char*)(vertexShaderFilePath.data()), vertexShaderFilePath.size() + 1);
	GLAssetIO::AppendField<char*>(writer.Get(), "FragmentShaderFilePath", (char*)(fragmentShaderFilePath.data()), fragmentShaderFilePath.size() + 1);
	unsigned int textureCount = linkedTextures.size();
	GLAssetIO::AppendField<unsigned int*>(writer.Get(), "TextureCount", &textureCount, sizeof(unsigned int));
	
	for (unsigned int i = 0; i < linkedTextures.size(); ++i)
	{
		GLAssetIO::AppendField<char*>(writer.Get(), "Texture_" + std::to_string(i), (char*)(linkedTextures[i].data()), linkedTextures[i].size() + 1);
	}
	writer.ForceCloseFile();
	new Material(newFilePath);
}
