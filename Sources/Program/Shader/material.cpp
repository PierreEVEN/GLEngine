#include "material.h"
#include "shaderLoader.h"
#include "../Asset/assetLibrary.h"
#include "../Texture/texture.h"
#include "../World/world.h"
#include "../Camera/camera.h"
#include <glfw3/glfw3.h>
#include "../Lighting/pointLight.h"
#include "../Lighting/spotLight.h"
#include "../Lighting/directionalLight.h"
#include "../Asset/AssetRegistry.h"
#include "../EngineLog/engineLog.h"
#include "../Engine/debugerTool.h"


GLuint ubo = 0;

Material::Material(std::string textAssetPath)
	: Asset(textAssetPath)
{
	SAssetReader reader(textAssetPath);
	if (!reader.IsValid()) return;

	SPropertyValue vertexShader(reader.Get(), "VertexShaderFilePath");
	SPropertyValue fragmentShader(reader.Get(), "FragmentShaderFilePath");
	SPropertyValue textureCount(reader.Get(), "TextureCount");

	if (!vertexShader.IsValid()) return;
	if (!fragmentShader.IsValid()) return;
	if (!textureCount.IsValid()) return;

	vertexShaderPath = vertexShader.GetValue<const char>();
	fragmentShaderPath = fragmentShader.GetValue<const char>();
	unsigned int texturesCount = *textureCount.GetValue<unsigned int>();

	for (unsigned int i = 0; i < texturesCount; ++i)
	{
		SPropertyValue textureName(reader.Get(), "Texture_" + std::to_string(i));
		if (!textureName.IsValid()) continue;
		linkedTexturesName.push_back(textureName.GetValue<const char>());
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
	LoadData();
	glUseProgram(ShaderID);

	unsigned int block_index = glGetUniformBlockIndex(ShaderID, "shader_data");
	GLuint binding_point_index = 2;
	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, ubo);
	glUniformBlockBinding(ShaderID, block_index, binding_point_index);

	glUniform3f(glGetUniformLocation(ShaderID, "CameraPosition"), (float)OwningWorld->GetCamera()->GetCameraLocation().X, (float)OwningWorld->GetCamera()->GetCameraLocation().Y, (float)OwningWorld->GetCamera()->GetCameraLocation().Z);
	setFloat("Time", (float)glfwGetTime());
	setFloat("MaterialShininess", 1.f);

	if (!bIsUnlit && !OwningWorld->bSkipLightRendering)
	{

		std::vector<PointLight*> pointLights = OwningWorld->GetPointLightSources();
		std::vector<DirectionalLight*> directionalLights = OwningWorld->GetDirectionalLightSources();
		std::vector<SpotLight*> spotLights = OwningWorld->GetSpotLightSources();
		for (unsigned int i = 0; i < pointLights.size() && i < 16; ++i)
		{
			setVec3("pointLights[" + std::to_string(i) + "].position", pointLights[i]->GetLocation().ToGLVector());
			setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLights[i]->ambiant[0], pointLights[i]->ambiant[1], pointLights[i]->ambiant[2]);
			setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLights[i]->diffuse[0], pointLights[i]->diffuse[1], pointLights[i]->diffuse[2]);
			setVec3("pointLights[" + std::to_string(i) + "].specular", pointLights[i]->specular[0], pointLights[i]->specular[1], pointLights[i]->specular[2]);
			setFloat("pointLights[" + std::to_string(i) + "].constant", pointLights[i]->constant);
			setFloat("pointLights[" + std::to_string(i) + "].linear", pointLights[i]->linear);
			setFloat("pointLights[" + std::to_string(i) + "].quadratic", pointLights[i]->quadratic);
		}
		for (unsigned int i = 0; i < directionalLights.size() && i < 2; ++i)
		{
			setVec3("dirLight[" + std::to_string(i) + "].ambient", directionalLights[i]->ambiant[0], directionalLights[i]->ambiant[1], directionalLights[i]->ambiant[2]);
			setVec3("dirLight[" + std::to_string(i) + "].diffuse", directionalLights[i]->diffuse[0], directionalLights[i]->diffuse[1], directionalLights[i]->diffuse[2]);
			setVec3("dirLight[" + std::to_string(i) + "].specular", directionalLights[i]->specular[0], directionalLights[i]->specular[1], directionalLights[i]->specular[2]);
			setVec3("dirLight[" + std::to_string(i) + "].direction", directionalLights[i]->direction);
		}
		for (unsigned int i = 0; i < spotLights.size() && i < 16; ++i)
		{
			setVec3("spotLight[" + std::to_string(i) + "].position", spotLights[i]->GetLocation().ToGLVector());
			setVec3("spotLight[" + std::to_string(i) + "].direction", spotLights[i]->direction);
			setVec3("spotLight[" + std::to_string(i) + "].ambient", spotLights[i]->ambiant[0], spotLights[i]->ambiant[1], spotLights[i]->ambiant[2]);
			setVec3("spotLight[" + std::to_string(i) + "].diffuse", spotLights[i]->diffuse[0], spotLights[i]->diffuse[1], spotLights[i]->diffuse[2]);
			setVec3("spotLight[" + std::to_string(i) + "].specular", spotLights[i]->specular[0], spotLights[i]->specular[1], spotLights[i]->specular[2]);
			setFloat("spotLight[" + std::to_string(i) + "].constant", spotLights[i]->constant);
			setFloat("spotLight[" + std::to_string(i) + "].linear", spotLights[i]->linear);
			setFloat("spotLight[" + std::to_string(i) + "].quadratic", spotLights[i]->quadratic);
			setFloat("spotLight[" + std::to_string(i) + "].cutOff", spotLights[i]->cutOff);
			setFloat("spotLight[" + std::to_string(i) + "].outerCutOff", spotLights[i]->outerCutOff);
		}
		setInt("DirectionalLightCounts", directionalLights.size());
		setInt("SpotLightCounts", spotLights.size());
		setInt("PointLightCounts", pointLights.size());
	}


	std::string name = "texture";
	for (unsigned int textureIndex = 0; textureIndex < textures.size(); ++textureIndex)
	{
		if (textures[textureIndex])
		{
			setInt(std::string("StaticTexture_") + std::to_string(textureIndex), textureIndex);
			glActiveTexture(GL_TEXTURE0 + textureIndex);
			glBindTexture(GL_TEXTURE_2D, textures[textureIndex]->GetTextureID());
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
