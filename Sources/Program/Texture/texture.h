#pragma once

#include <string>
#include "../Asset/asset.h"


class Texture2D : public Asset
{
	unsigned int textureID = -1;

public:

	Texture2D(std::string textAssetPath);

	virtual unsigned int GetTextureID();

	virtual void ImportData() override;
	virtual void OnPropertiesLoaded() override;

	virtual void BuildThumbnail() override;
	virtual ImColor GetAssetColor() { return ImColor(0.3f, 0.3f, 0.7f, 1.f); }
	virtual void OnAssetClicked() override;
};

class TextureCube : public Texture2D
{

	unsigned int textureCubeID = -1;

	virtual void ImportData() override;
	virtual void OnPropertiesLoaded() override;

public:

	virtual unsigned int GetTextureID() override;

	TextureCube(std::string textAssetPath);

	virtual ImColor GetAssetColor() { return ImColor(1.f, 0.1f, 0.1f, 1.f); }
	virtual void BuildThumbnail() override;
};