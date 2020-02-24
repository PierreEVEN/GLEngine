#pragma once

#include <EnginePCH.h>
#include <Lighting/Light.h>
#include <Lighting/directionalLight.h>
#include <Lighting/pointLight.h>
#include <Lighting/spotLight.h>
#include <Asset/asset.h>

#define MAX_DIRECTIONAL_LIGHTS 16
#define MAX_POINT_LIGHTS 512
#define MAX_SPOT_LIGHTS 64

class World;
struct LightParameters;
struct DirectionalLightParameters;
struct PointLightParameters;
struct SpotLightParameters;

struct DefaultShaderData
{
	glm::mat4 viewMatrix;
	glm::mat4 worldProjection;
	glm::vec4 cameraLocation;
	int directionalLightCount;
	int pointLightCount;
	int spotLightCount;
	int offset;
 	LightParameters directionalLightBaseParams[MAX_DIRECTIONAL_LIGHTS];
	DirectionalLightParameters directionalLightAdvancedParams[MAX_DIRECTIONAL_LIGHTS];
	LightParameters pointLightBaseParameters[MAX_POINT_LIGHTS];
	PointLightParameters pointLightAdvancedParameters[MAX_POINT_LIGHTS];
	LightParameters spotLightBaseParameters[MAX_SPOT_LIGHTS];
	SpotLightParameters spotLightAdvancedParameters[MAX_SPOT_LIGHTS];
};

class Scene;
class ShaderLoader;

class Material : public Asset
{
private:

	ShaderLoader* compiler = nullptr;
public:
	unsigned int ShaderID;

	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::vector<Texture2D*> textures;

	bool bIsUnlit;

	Material(std::string textAssetPath);

	virtual void LoadProperties() override;
	virtual void PostLoadProperties() override;

	void AddTexture(Texture2D* inTexture = nullptr);
	unsigned int GetTextureCount();

	void use();
	static void InitializeMaterials();
	static void UpdateMaterialDefaults(Scene* drawScene);
	void UpdateLinkedTextures();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setVec2(const std::string &name, float x, float y) const;

	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;

	void setVec4(const std::string &name, const glm::vec4 &value) const;
	void setVec4(const std::string &name, float x, float y, float z, float w) const;

	void setMat2(const std::string &name, const glm::mat2 &mat) const;
	void setMat3(const std::string &name, const glm::mat3 &mat) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

	virtual ImColor GetAssetColor();
	virtual void OnAssetClicked() override;
	virtual void SaveAsset() override;
	virtual void BuildThumbnail();
};

class MaterialEditorDebuger
{
public:
	static Material* GetDebugMaterial();
	static Material* GetGridMaterial();
	static Material* GetGizmoMaterial();
};