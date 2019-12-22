#include "material.h"
#include "shaderLoader.h"
#include "../Asset/assetLibrary.h"
#include "../Texture/texture.h"
#include "../World/world.h"
#include "../Camera/camera.h"
#include <glfw3/glfw3.h>
#include "../Asset/AssetRegistry.h"
#include "../EngineLog/engineLog.h"
#include "../Engine/debugerTool.h"

GLuint ubo = 0;

Material::Material(std::string textAssetPath)
	: Asset(textAssetPath)
{
	SAssetReader reader(textAssetPath);
	if (!reader.IsValid()) return;

	SFileRefPropertyValue* vertexShader = new SFileRefPropertyValue(this, reader.Get(), "VertexShaderFilePath");
	SFileRefPropertyValue* fragmentShader = new SFileRefPropertyValue(this, reader.Get(), "FragmentShaderFilePath");
	SUIntPropertyValue* textureCount = new SUIntPropertyValue(this, reader.Get(), "TextureCount");

	RegisterProperty(vertexShader);
	RegisterProperty(fragmentShader);
	RegisterProperty(textureCount);

	if (!vertexShader->IsValid()) return;
	if (!fragmentShader->IsValid()) return;
	if (!textureCount->IsValid()) return;

	vertexShaderPath = vertexShader->GetFileRef();
	fragmentShaderPath = fragmentShader->GetFileRef();
	unsigned int texturesCount = textureCount->GetUIntValue();

	for (unsigned int i = 0; i < texturesCount; ++i)
	{
		SAssetRefPropertyValue* textureName = new SAssetRefPropertyValue(this, reader.Get(), "Texture_" + std::to_string(i));
		if (!RegisterProperty(textureName)) continue;
		linkedTexturesName.push_back(textureName->GetAssetRef());
	}
}

void Material::InitializeShader(const char* inVertexShaderPath, const char* inFragmentShaderPath, std::vector<std::string> newTextures)
{
	ShaderLoader* compiler = new ShaderLoader(inVertexShaderPath, inFragmentShaderPath);
	ShaderID = compiler->Get();

	for (const auto& texture : newTextures)
	{
		if (Texture2D* foundTexture = AssetRegistry::FindAssetByName<Texture2D>(texture))
		{
			textures.push_back(foundTexture);
		}
		else
		{
			GLog(LogVerbosity::Error, "Material", "failed to find texture " + texture + " for material " + GetName());
		}
	}

	for (unsigned int textureIndex = 0; textureIndex < textures.size(); ++textureIndex)
	{
		if (textures[textureIndex])
		{
			setInt("texture" + textureIndex, textures[textureIndex]->GetTextureID());
		}
	}
}

void Material::ImportData()
{
	Asset::ImportData();
	InitializeShader(vertexShaderPath.data(), fragmentShaderPath.data(), linkedTexturesName);

}

void Material::use(World* OwningWorld)
{
	{
		LoadData();
		glUseProgram(ShaderID);
		ProfileStat("UBO Binding");
		unsigned int block_index = glGetUniformBlockIndex(ShaderID, "shader_data");
		GLuint binding_point_index = 0;
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, ubo);
		glUniformBlockBinding(ShaderID, block_index, binding_point_index);
	}

	{
		ProfileStat("set shader textures");
		for (unsigned int textureIndex = 0; textureIndex < textures.size(); ++textureIndex)
		{
			if (textures[textureIndex])
			{
				ProfileStat("Set texture ID");
				setInt(std::string("StaticTexture_") + std::to_string(textureIndex), textureIndex);
				StatViewer::AddDrawcall();
				glActiveTexture(GL_TEXTURE0 + textureIndex);
				glBindTexture(GL_TEXTURE_2D, textures[textureIndex]->GetTextureID());
			}
		}
	}
}

void Material::InitializeMaterials()
{
	DefaultShaderData shader_data;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Material::UpdateMaterialDefaults(World* OwningWorld)
{
	DefaultShaderData shader_data;
	shader_data.cameraLocation = OwningWorld->GetCamera()->GetCameraLocation().ToGLVector();
	shader_data.viewMatrix = OwningWorld->GetCamera()->GetViewMatrix();
	shader_data.worldProjection = OwningWorld->GetProjection();

	std::vector<PointLight*> pointLights = OwningWorld->GetPointLightSources();
	std::vector<DirectionalLight*> directionalLights = OwningWorld->GetDirectionalLightSources();
	std::vector<SpotLight*> spotLights = OwningWorld->GetSpotLightSources();
	shader_data.directionalLightCount = directionalLights.size();
	shader_data.pointLightCount = pointLights.size();
	shader_data.spotLightCount = spotLights.size();
	for (unsigned int i = 0; i < directionalLights.size() && i < MAX_DIRECTIONAL_LIGHTS; ++i)
	{
		shader_data.directionalLightBaseParams[i] = directionalLights[i]->lightParams;
		shader_data.directionalLightAdvancedParams[i] = directionalLights[i]->directionalLightParams;
	}
	for (unsigned int i = 0; i < pointLights.size() && i < MAX_POINT_LIGHTS; ++i)
	{
		shader_data.pointLightBaseParameters[i] = pointLights[i]->lightParams;
		shader_data.pointLightAdvancedParameters[i] = pointLights[i]->pointLightParams;
	}
	for (unsigned int i = 0; i < spotLights.size() && i < MAX_SPOT_LIGHTS; ++i)
	{
		shader_data.spotLightBaseParameters[i] = spotLights[i]->lightParams;
		shader_data.spotLightAdvancedParameters[i] = spotLights[i]->spotLightParams;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &shader_data, sizeof(shader_data));
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void Material::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), (int)value);
}

void Material::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), value);
}

void Material::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ShaderID, name.c_str()), value);
}

void Material::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(ShaderID, name.c_str()), 1, &value[0]);
}

void Material::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ShaderID, name.c_str()), x, y);
}

void Material::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ShaderID, name.c_str()), 1, &value[0]);
}

void Material::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ShaderID, name.c_str()), x, y, z);
}

void Material::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(ShaderID, name.c_str()), 1, &value[0]);
}

void Material::setVec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ShaderID, name.c_str()), x, y, z, w);
}

void Material::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ShaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Material::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ShaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Material::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ShaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

Texture2D* Material::GetAssetThumbnail()
{
	LoadData();
	if (textures.size() > 0)
	{
		return textures[0];
	}
	return nullptr;
}

Material* debugMaterial = nullptr;
Material* gridMaterial = nullptr;

Material* MaterialEditorDebuger::GetDebugMaterial()
{
	if (!debugMaterial)
	{
		debugMaterial = AssetRegistry::FindAssetByName<Material>("DebugViewMaterial");
		assert(debugMaterial);
	}
	return debugMaterial;
}

Material* MaterialEditorDebuger::GetGridMaterial()
{
	if (!gridMaterial)
	{
		gridMaterial = AssetRegistry::FindAssetByName<Material>("DebugGridMaterial");
		assert(gridMaterial);
	}
	return gridMaterial;
}
