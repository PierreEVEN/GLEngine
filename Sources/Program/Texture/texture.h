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

	Texture2D(std::string textAssetPath) : Asset(textAssetPath) { }

	unsigned int GetTextureID() const { return textureID; }
	
};
#endif