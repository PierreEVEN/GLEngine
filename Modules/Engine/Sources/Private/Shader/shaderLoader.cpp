
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glad.h>
#include <Shader/shaderLoader.h>
#include "EngineLog/engineLog.h"

ShaderLoader::ShaderLoader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexShaderPath);
		fShaderFile.open(fragmentShaderPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		GFullLog(LogVerbosity::Error, "AssetRegistry", "Failed to read shader file '" + std::string(vertexShaderPath) + "' | '" + fragmentShaderPath + "'");
	}
}

int ShaderLoader::Get()
{
	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;


	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	ShaderID = glCreateProgram();
	glAttachShader(ShaderID, vertex);
	glAttachShader(ShaderID, fragment);
	glLinkProgram(ShaderID);

	checkCompileErrors(ShaderID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return ShaderID;
}

void ShaderLoader::checkCompileErrors(unsigned int shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			GFullLog(LogVerbosity::Error, "AssetRegistry", "Failed to compile shader " + std::string(type) + " : " + std::string(infoLog));
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			GFullLog(LogVerbosity::Error, "AssetRegistry", "Failed to compile shader " + std::string(type) + " : " + std::string(infoLog));
		}
	}
}