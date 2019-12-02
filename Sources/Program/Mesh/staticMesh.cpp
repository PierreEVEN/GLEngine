#include "staticMesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "../Asset/assetLibrary.h"
#include "../Shader/material.h"
#include "../ThirdParty/stb_image.h"
#include <assimp/material.h>


void StaticMesh::LoadData(std::string path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	std::string directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void StaticMesh::processNode(aiNode *node, const aiScene *scene)
{
	if (!node) return;
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		if (aiMesh* mesh = scene->mMeshes[node->mMeshes[i]])
		{
			meshSections.push_back(processMesh(mesh, scene, i));
		}
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
			processNode(node->mChildren[i], scene);
	}
}

StaticMeshSection StaticMesh::processMesh(aiMesh *mesh, const aiScene *scene, unsigned int meshIndex)
{
	// data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{

		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
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
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		// tangent
		vector.x = mesh->mTangents ? mesh->mTangents[i].x : 0.f;
		vector.y = mesh->mTangents ? mesh->mTangents[i].y : 0.f;
		vector.z = mesh->mTangents ? mesh->mTangents[i].z : 0.f;
		vertex.Tangent = vector;
		// bitangent
		vector.x = mesh->mBitangents ? mesh->mBitangents[i].x : 0.f;
		vector.y = mesh->mBitangents ? mesh->mBitangents[i].y : 0.f;
		vector.z = mesh->mBitangents ? mesh->mBitangents[i].z : 0.f;
		vertex.Bitangent = vector;
		vertices.push_back(vertex);
	}

	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture2D*> materialTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE);


	// return a mesh object created from the extracted mesh data
	Material* sectionMaterial = nullptr;
	if (usedMaterial.size() > meshIndex)
	{
		sectionMaterial = usedMaterial[meshIndex];
	}
	return StaticMeshSection(vertices, indices, sectionMaterial, materialTextures);
}


std::vector<Texture2D*> StaticMesh::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
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