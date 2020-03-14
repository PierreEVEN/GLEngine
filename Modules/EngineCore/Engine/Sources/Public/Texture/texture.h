#pragma once

#include <string>
#include "../Asset/asset.h"


class Texture2D : public Asset
{
	unsigned int textureID = -1;


protected:

	bool bIsGPUDataCreated = false;
	virtual void CreateGPUData();

public:

	Texture2D(std::string textAssetPath);

	virtual unsigned int GetTextureID();

	virtual void LoadProperties() override;

	virtual void BuildThumbnail() override;
	virtual ImColor GetAssetColor();
	virtual void OnAssetClicked() override;
};

class TextureCube : public Texture2D
{

	unsigned int textureCubeID = -1;

	virtual void LoadProperties() override;

	virtual void CreateGPUData() override;

public:

	virtual unsigned int GetTextureID() override;

	TextureCube(std::string textAssetPath);

	virtual ImColor GetAssetColor() { return ImColor(1.f, 0.1f, 0.1f, 1.f); }
	virtual void BuildThumbnail() override;
};