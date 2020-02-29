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
	unsigned int ShaderID = -1;
public:

	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::vector<Texture2D*> textures;

	bool bIsUnlit;

	Material(std::string textAssetPath);

	unsigned int GetShaderID();

	virtual void LoadProperties() override;

	void AddTexture(Texture2D* inTexture = nullptr);
	unsigned int GetTextureCount();

	void use();
	static void InitializeMaterials();
	static void UpdateMaterialDefaults(Scene* drawScene);
	void UpdateLinkedTextures();

	void setBool(const std::string &name, bool value);
	void setInt(const std::string &name, int value);
	void setFloat(const std::string &name, float value);

	void setVec2(const std::string &name, const glm::vec2 &value);
	void setVec2(const std::string &name, float x, float y);

	void setVec3(const std::string &name, const glm::vec3 &value);
	void setVec3(const std::string &name, float x, float y, float z);

	void setVec4(const std::string &name, const glm::vec4 &value);
	void setVec4(const std::string &name, float x, float y, float z, float w);

	void setMat2(const std::string &name, const glm::mat2 &mat);
	void setMat3(const std::string &name, const glm::mat3 &mat);
	void setMat4(const std::string &name, const glm::mat4 &mat);

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