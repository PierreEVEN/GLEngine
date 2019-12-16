#ifndef TEXTURE_H
#define TEXTURE_H


#include <string>
#include "../Asset/asset.h"


class Texture2D : public Asset
{

	unsigned int textureID;
	std::string textureDataPath;

	void LoadFromPath(std::string textAssetPath);

public:

	Texture2D(std::string textAssetPath);

	unsigned int GetTextureID();

	virtual void ImportData() override;

	virtual Texture2D* GetAssetThumbnail() override { return this; }
	virtual ImColor GetAssetColor() { return ImColor(0.3f, 0.3f, 0.7f, 1.f); }
};
#endif