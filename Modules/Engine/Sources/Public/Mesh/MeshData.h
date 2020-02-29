
#pragma once

#include <EnginePCH.h>
#include <Scene/primitiveComponent.h>

class Material;
class Texture2D;
class SceneMeshProxy;

struct SVertex
{
	SVertex() {}
	SVertex(glm::vec3 inPosition) : Position(inPosition) {}
	SVertex(glm::vec3 inPosition, glm::vec4 inVertexColor) : Position(inPosition), VertexColor(inVertexColor) {}
	SVertex(glm::vec3 inPosition, glm::vec3 inNormal, glm::vec2 inTexCoords, glm::vec3 inTangent, glm::vec3 inBitangent, glm::vec4 inVertexColor)
		: Position(inPosition), Normal(inNormal), TexCoords(inTexCoords), Tangent(inTangent), Bitangent(inBitangent), VertexColor(inVertexColor) {}

	glm::vec3 Position = glm::vec3(0);
	glm::vec3 Normal = glm::vec3(0);
	glm::vec2 TexCoords = glm::vec2(0);
	glm::vec3 Tangent = glm::vec3(0);
	glm::vec3 Bitangent = glm::vec3(0);
	glm::vec4 VertexColor = glm::vec4(0);
};


/************************************************************************/
/* Proxy Material Group                                                 */
/************************************************************************/

/** Contains material data of only one section */
struct SProxyMaterialData {
	SProxyMaterialData(const unsigned int& inSectionIndex, Material* inMaterial) :
		SectionIndex(inSectionIndex),
		SectionMaterial(inMaterial)
	{}

	FORCEINLINE void SetMaterial(Material* inMaterial) { SectionMaterial = inMaterial; }
	FORCEINLINE Material* GetMaterial() const { return SectionMaterial; }
	FORCEINLINE unsigned int GetSectionIndex() const { return SectionIndex; }
private:
	unsigned int SectionIndex;
	Material* SectionMaterial;
};

/** Contain material data for only one lod level */
struct SProxyMaterialGroup {
	SProxyMaterialGroup(const unsigned int& inLodLevel, const std::vector<SProxyMaterialData>& inMaterials) :
		LodLevel(inLodLevel),
		Materials(inMaterials)
	{}
	
	void SetMaterial(const unsigned int& SectionIndex, Material* newMaterial) {
		for (auto& material : Materials) {
			if (SectionIndex == material.GetSectionIndex()) {
				material.SetMaterial(newMaterial);
				return;
			}
		}
		Materials.push_back(SProxyMaterialData(SectionIndex, newMaterial));
	}
	FORCEINLINE Material* GetMaterial(const unsigned int& SectionIndex) const {
		for (const auto& material : Materials) {
			if (material.GetSectionIndex() == SectionIndex) return material.GetMaterial();
		}
		return nullptr;
	}
	FORCEINLINE unsigned int GetLodLevel() const { return LodLevel; }

	void CopyFromAdditive(const SProxyMaterialGroup& inMaterialData) {
		for (const auto& material : inMaterialData.Materials) {
			if (!GetMaterial(material.GetSectionIndex())) {
				SetMaterial(material.GetSectionIndex(), material.GetMaterial());
			}
		}
	}
private:
	unsigned int LodLevel;
	std::vector<SProxyMaterialData> Materials;
};

/************************************************************************/
/* Proxy Mesh Group                                                     */
/************************************************************************/

/** Contains mesh data of only one section */
struct SProxySectionData {
	SProxySectionData(const unsigned int& inVBO, const unsigned int& inVAO, const unsigned int& inEBO, const unsigned int& inTriangleCount, const unsigned int& inSectionIndex) :
		SectionIndex(inSectionIndex),
		VBO(inVBO),
		VAO(inVAO),
		EBO(inEBO),
		TriangleCount(inTriangleCount)
	{}

	FORCEINLINE unsigned int GetVao() const { return VAO; }
	FORCEINLINE unsigned int GetTriangleCount() const { return TriangleCount; }
	FORCEINLINE unsigned int GetSectionIndex() const { return SectionIndex; }

private:
	unsigned int SectionIndex;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	unsigned int TriangleCount;
};

/** Contain mesh data for only one lod level */
struct SProxySectionGroup {
	SProxySectionGroup(const unsigned int& inLodLevel, const std::vector<SProxySectionData>& inSections) :
		LodLevel(inLodLevel),
		Sections(inSections)
	{}

	void SetMeshData(const unsigned int& SectionIndex, const SProxySectionData& newMeshData) {
		for (auto& section : Sections) {
			if (SectionIndex == section.GetSectionIndex()) {
				section = newMeshData;
				return;
			}
		}
		Sections.push_back(newMeshData);
	}
	FORCEINLINE unsigned int GetVao(const unsigned int& inSectionIndex) const {
		for (const auto& section : Sections) {
			if (section.GetSectionIndex() == inSectionIndex) {
				return section.GetVao();
			}
		}
		return -1;
	}
	FORCEINLINE unsigned int GetTriangleCount(const unsigned int& inSectionIndex) const {
		for (const auto& section : Sections) {
			if (section.GetSectionIndex() == inSectionIndex) {
				return section.GetTriangleCount();
			}
		}
		return -1;
	}
	FORCEINLINE unsigned int GetLodLevel() const { return LodLevel; }

	void CopyFromAdditive(const SProxySectionGroup& inMeshData) {
		for (const auto& meshData : inMeshData.Sections) {
			if (GetVao(meshData.GetSectionIndex()) == -1) {
				SetMeshData(meshData.GetSectionIndex(), meshData);
			}
		}
	}
private:
	unsigned int LodLevel;
	std::vector<SProxySectionData> Sections;
};

/************************************************************************/
/* Proxy Lod group                                                      */
/************************************************************************/

/** Contains multiple LODData */
struct SProxyLodGroup {
	SProxyLodGroup() :
		ProxyLodsMesh({}),
		ProxyLodMaterials({})
	{}

	void SetMeshData(const unsigned int& inSectionIndex, const SProxySectionData& inMeshData, const unsigned int& inLodIndex = 0) {
		for (auto& meshLod : ProxyLodsMesh) {
			if (meshLod.GetLodLevel() == inLodIndex) {
				meshLod.SetMeshData(inSectionIndex, inMeshData);
				bHasDataBeenChangedRenderThread = true;
				bIsEmpty = false;
				return;
			}
		}
		ProxyLodsMesh.push_back(SProxySectionGroup(inLodIndex, { inMeshData }));
		bHasDataBeenChangedRenderThread = true;
		bIsEmpty = false;
		return;
	}

	void SetMaterial(const unsigned int& inSectionIndex, Material* inMaterial, const unsigned int& inLodIndex = 0) {
		for (auto& materialLod : ProxyLodMaterials) {
			if (materialLod.GetLodLevel() == inLodIndex) {
				materialLod.SetMaterial(inSectionIndex, inMaterial);
				bHasDataBeenChangedRenderThread = true;
				return;
			}
		}
		ProxyLodMaterials.push_back(SProxyMaterialGroup(inLodIndex, { SProxyMaterialData(inSectionIndex, inMaterial) }));
		bHasDataBeenChangedRenderThread = true;
		return;
	}
	FORCEINLINE Material* GetMaterial(const unsigned int& inLodIndex, const unsigned int& inSectionIndex) const {
		for (const auto& MaterialLod : ProxyLodMaterials) {
			if (MaterialLod.GetLodLevel() == inLodIndex) {
				return MaterialLod.GetMaterial(inSectionIndex);
			}
		}
		return nullptr;
	}
	FORCEINLINE unsigned int GetVao(const unsigned int& inLodIndex, const unsigned int& inSectionIndex) const {
		for (const auto& Lod : ProxyLodsMesh) {
			if (Lod.GetLodLevel() == inLodIndex) {
				return Lod.GetVao(inSectionIndex);
			}
		}
		return -1;
	}
	FORCEINLINE unsigned int GetTriangleCount(const unsigned int& inLodIndex, const unsigned int& inSectionIndex) const {
		for (const auto& Lod : ProxyLodsMesh) {
			if (Lod.GetLodLevel() == inLodIndex) {
				return Lod.GetTriangleCount(inSectionIndex);
			}
		}
		return -1;
	}
	FORCEINLINE bool HasDataBeenChangedRenderThread() const { return bHasDataBeenChangedRenderThread; }
	FORCEINLINE bool GetLodCount() const {
		unsigned int max = 0;
		for (const auto& lod : ProxyLodsMesh) {
			if (lod.GetLodLevel() + 1 > max) max = lod.GetLodLevel() + 1;
		}
		return max;
	}
	FORCEINLINE bool IsEmpty() const { return bIsEmpty; }

	void CopyDataToSceneMeshProxy(SceneMeshProxy* inProxy);
	void CopyFromAdditive(const SProxyLodGroup& inMeshData)
	{
		ProxyLodsMesh = inMeshData.ProxyLodsMesh;
		ProxyLodMaterials = inMeshData.ProxyLodMaterials;
		bHasDataBeenChangedRenderThread = true;
		bIsEmpty = false;
	}

private:

	bool bHasDataBeenChangedRenderThread = true;
	bool bIsEmpty = true;
	std::vector<SProxySectionGroup> ProxyLodsMesh;
	std::vector<SProxyMaterialGroup> ProxyLodMaterials;
};



















struct MeshSectionData
{
	MeshSectionData(SVertex* newSectionVertices, unsigned int inVerticesCount, unsigned int* newSectionIndices, unsigned int inIndicesCount, Material* usedMaterial, const std::vector<Texture2D*>& usedTextures)
		: sectionVertices(newSectionVertices), verticesCount(inVerticesCount), sectionIndices(newSectionIndices), indicesCount(inIndicesCount), material(usedMaterial), textures(usedTextures)
	{
		ComputeBounds();
	}
	SVertex* sectionVertices;
	unsigned int verticesCount;
	unsigned int* sectionIndices;
	unsigned int indicesCount;
	Material* material;
	std::vector<Texture2D*> textures;
	BoxSphereBound localBounds;

	void ComputeBounds();
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