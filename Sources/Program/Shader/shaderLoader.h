#pragma once

#include <glad/glad.h>
#include <string>

class ShaderLoader
{
public:
	unsigned int ShaderID;
	
	std::string vertexCode;
	std::string fragmentCode;

	ShaderLoader(const char* vertexShaderPath, const char* fragmentShaderPath);


	int Get();

private:

	void checkCompileErrors(GLuint shader, std::string type);
};