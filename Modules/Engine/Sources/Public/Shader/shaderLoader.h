#pragma once

#include <EnginePCH.h>

class ShaderLoader
{
public:
	unsigned int ShaderID;
	
	std::string vertexCode;
	std::string fragmentCode;

	ShaderLoader(const char* vertexShaderPath, const char* fragmentShaderPath);


	int Get();

private:

	void checkCompileErrors(unsigned int shader, std::string type);
};