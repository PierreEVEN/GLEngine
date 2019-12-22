#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "../Texture/texture.h"
#include "../Asset/asset.h"
#include "../Lighting/directionalLight.h"
#include "../Lighting/pointLight.h"
#include "../Lighting/spotLight.h"

#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_POINT_LIGHTS 16
#define MAX_SPOT_LIGHTS 16

class World;
struct LightParameters;
struct DirectionalLightParameters;
struct PointLightParameters;
struct SpotLightParameters;

struct DefaultShaderData
{
	glm::mat4 viewMatrix;
	glm::mat4 worldProjection;
	glm::vec3 cameraLocation;
	int directionalLightCount;
	int pointLightCount;
	int spotLightCount;
 	LightParameters directionalLightBaseParams[MAX_DIRECTIONAL_LIGHTS];
	DirectionalLightParameters directionalLightAdvancedParams[MAX_DIRECTIONAL_LIGHTS];
	LightParameters pointLightBaseParameters[MAX_POINT_LIGHTS];
	PointLightParameters pointLightAdvancedParameters[MAX_POINT_LIGHTS];
	LightParameters spotLightBaseParameters[MAX_SPOT_LIGHTS];
	SpotLightParameters spotLightAdvancedParameters[MAX_SPOT_LIGHTS];
};

class Material : public Asset
{
public:
	unsigned int ShaderID;

	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::vector<std::string> linkedTexturesName;
	std::vector<Texture2D*> textures;

	bool bIsUnlit;

	Material(std::string textAssetPath);

	void InitializeShader(const char* inVertexShaderPath, const char* inFragmentShaderPath, std::vector<std::string> newTextures);

	virtual void ImportData() override;

	void use(World* OwningWorld);
	static void InitializeMaterials();
	static void UpdateMaterialDefaults(World* OwningWorld);

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

	virtual Texture2D* GetAssetThumbnail() override;
	virtual ImColor GetAssetColor() { return ImColor(0.3f, 0.6f, 0.3f, 1.f); }
};

class MaterialEditorDebuger
{
public:
	static Material* GetDebugMaterial();
	static Material* GetGridMaterial();
};