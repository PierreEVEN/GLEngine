#ifndef IMPORTERWINDOWS_H
#define IMPORTERWINDOWS_H

#include <string>
#include <UI/FloatingWindow.h>

class World;
class Material;
class Texture2D;

class MeshImporterWindow : public ImFloatingWindow
{
public:
	MeshImporterWindow(std::string inWindowName) : ImFloatingWindow(inWindowName) {}

	virtual void Draw(World* inWorld) override;
	std::string sourceFilePath;
	std::string outputFilePath;
	bool bImportTextures = false;

	std::vector<Material*> selectedMaterials;

	char outAssetNameString[32] = "";

	void CreateObject();
};

class TextureImporterWindow : public ImFloatingWindow
{
public:

	TextureImporterWindow(std::string inWindowName) : ImFloatingWindow(inWindowName) {}

	virtual void Draw(World* inWorld) override;
	std::string sourceFilePath;
	std::string outputFilePath;

	char outAssetNameString[32] = "";

	void CreateObject();
};

class TextureCubeImporterWindow : public ImFloatingWindow
{
public:

	TextureCubeImporterWindow(std::string inWindowName) : ImFloatingWindow(inWindowName) {}

	virtual void Draw(World* inWorld) override;
	std::string rightTexturePath;
	std::string leftTexturePath;
	std::string topTexturePath;
	std::string bottomTexturePath;
	std::string frontTexturePath;
	std::string backTexturePath;
	std::string outputFilePath;

	char outAssetNameString[32] = "";

	void CreateObject();
};

class ShaderImporterWindow : public ImFloatingWindow
{
public:

	ShaderImporterWindow(std::string inWindowName) : ImFloatingWindow(inWindowName) {}
	virtual void Draw(World* inWorld) override;
	std::string vertexShadersPath;
	std::string fragmentShadersPath;
	std::string outputFilePath;

	std::vector<Texture2D*> selectedTextures;

	char outAssetNameString[32] = "";

	void CreateObject();
};
#endif