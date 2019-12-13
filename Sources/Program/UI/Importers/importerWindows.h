#ifndef IMPORTERWINDOWS_H
#define IMPORTERWINDOWS_H

#include "../EditorWindow.h"
#include <string>
#include "../EditorWindows/assetSelector.h"

class World;
class Material;
class Texture2D;

class MeshImporterWindow : public UIWindowElement
{
public:
	MeshImporterWindow(std::string inWindowName) : UIWindowElement(inWindowName) {}

	virtual void Draw(World* inWorld) override;
	std::string sourceFilePath;
	std::string outputFilePath;
	bool bImportTextures = false;

	std::vector<Material*> selectedMaterials;

	char outAssetNameString[32] = "";

	void CreateObject();
};

class TextureImporterWindow : public UIWindowElement
{
public:

	TextureImporterWindow(std::string inWindowName) : UIWindowElement(inWindowName) {}

	virtual void Draw(World* inWorld) override;
	std::string sourceFilePath;
	std::string outputFilePath;

	char outAssetNameString[32] = "";

	void CreateObject();
};

class ShaderImporterWindow : public UIWindowElement
{
public:

	ShaderImporterWindow(std::string inWindowName) : UIWindowElement(inWindowName) {}
	virtual void Draw(World* inWorld) override;
	std::string vertexShadersPath;
	std::string fragmentShadersPath;
	std::string outputFilePath;

	std::vector<Texture2D*> selectedTextures;

	char outAssetNameString[32] = "";

	void CreateObject();
};
#endif