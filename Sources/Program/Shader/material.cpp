#include "material.h"
#include "shaderLoader.h"

Material::Material(const char* vertexShaderPath, const char* fragmentShaderPath, std::vector<Texture2D*> newTextures)
{
	ShaderLoader* compiler = new ShaderLoader(vertexShaderPath, fragmentShaderPath);
	ShaderID = compiler->Get();
	textures = newTextures;

	for (unsigned int textureIndex = 0; textureIndex < newTextures.size(); ++textureIndex)
	{
		setInt("texture" + textureIndex, newTextures[textureIndex]->GetTextureID());
	}
}

void Material::use() const
{
	std::string name = "texture";
	for (unsigned int textureIndex = 0; textureIndex < textures.size(); ++textureIndex)
	{
// 		glActiveTexture(GL_TEXTURE0 + textures[textureIndex]->GetTextureID());
// 		glBindTexture(GL_TEXTURE_2D, textures[textureIndex]->GetTextureID()	
		glUniform1i(glGetUniformLocation(ShaderID, (name + std::to_string(textureIndex)).c_str()), textureIndex);
		glBindTexture(GL_TEXTURE_2D, textures[textureIndex]->GetTextureID());
	}

	glUseProgram(ShaderID);
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