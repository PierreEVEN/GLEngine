#include "material.h"
#include "shaderLoader.h"
#include "../Asset/assetLibrary.h"
#include "../Texture/texture.h"
#include "../World/world.h"
#include "../Camera/camera.h"
#include <glfw3/glfw3.h>

Material::Material(const char* vertexShaderPath, const char* fragmentShaderPath, std::vector<Texture2D*> newTextures)
{
	InitializeShader(vertexShaderPath, fragmentShaderPath, newTextures);
}



void Material::InitializeShader(const char* vertexShaderPath, const char* fragmentShaderPath, std::vector<Texture2D*> newTextures)
{
	ShaderLoader* compiler = new ShaderLoader(vertexShaderPath, fragmentShaderPath);
	ShaderID = compiler->Get();
	textures = newTextures;

	for (unsigned int textureIndex = 0; textureIndex < newTextures.size(); ++textureIndex)
	{
		if (newTextures[textureIndex])
		{
			setInt("texture" + textureIndex, newTextures[textureIndex]->GetTextureID());
		}
	}
}

void Material::use(World* OwningWorld) const
{
	glUseProgram(ShaderID);


	glUniform3f(glGetUniformLocation(ShaderID, "CameraPosition"), OwningWorld->GetCamera()->GetCameraLocation()[0], OwningWorld->GetCamera()->GetCameraLocation()[1], OwningWorld->GetCamera()->GetCameraLocation()[2]);
	glUniform3f(glGetUniformLocation(ShaderID, "LightDirection"), .5, -.5, .5);
	setFloat("Time", glfwGetTime());

	std::string name = "texture";
	for (unsigned int textureIndex = 0; textureIndex < textures.size(); ++textureIndex)
	{
		if (textures[textureIndex])
		{
			setInt(std::string("StaticTexture_") + std::to_string(textureIndex), textureIndex);
			glUniform1i(glGetUniformLocation(ShaderID, (name + std::to_string(textureIndex)).c_str()), textureIndex);
			glBindTexture(GL_TEXTURE_2D, textures[textureIndex]->GetTextureID());
		}
	}
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

void Material::Parse(const Document& data)
{
	Asset::Parse(data);
	assert(data.HasMember("VertexShaderPath"));
	assert(data["VertexShaderPath"].IsString());
	vertexShaderPath = data["VertexShaderPath"].GetString();

	assert(data.HasMember("FragmentShaderPath"));
	assert(data["FragmentShaderPath"].IsString());
	fragmentShaderPath = data["FragmentShaderPath"].GetString();

	textures = {};
	if (data.HasMember("Textures"))
	{
		const Value& textureData = data["Textures"];
		assert(textureData.IsArray());
		for (SizeType textureIndex = 0; textureIndex < textureData.Size(); textureIndex++)
		{
			assert(textureData[textureIndex].IsString());
			if (Texture2D* texture2DAsset = AssetLibrary::FindAssetByName<Texture2D>(textureData[textureIndex].GetString()))
			{
				textures.push_back(texture2DAsset);
			}
			else
			{
				std::cout << textureData[textureIndex].GetString() << " is not a valid texture file" << std::endl;
			}
		}
	}
	InitializeShader(vertexShaderPath.data(), fragmentShaderPath.data(), textures);
}