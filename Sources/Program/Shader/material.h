#ifndef MATERIAL_H
#define MATERIAL_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "../Texture/texture.h"
#include "../Asset/asset.h"

class Material : public Asset
{
public:

	unsigned int ShaderID;

	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::vector<Texture2D*> textures;

	Material(std::string textAssetPath) : Asset(textAssetPath) { }

	Material(const char* vertexShaderPath, const char* fragmentShaderPath, std::vector<Texture2D*> newTextures);

	void Load(const char* vertexShaderPath, const char* fragmentShaderPath, std::vector<Texture2D*> newTextures);

	virtual void Parse(const Document& data) override;

	void use() const;

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
};
#endif