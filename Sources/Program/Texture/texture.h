#ifndef TEXTURE_H
#define TEXTURE_H


#include <string>


class Texture2D
{

	unsigned int textureID;

public:

	Texture2D(std::string texturePath);

	unsigned int GetTextureID() const { return textureID; }
};
#endif