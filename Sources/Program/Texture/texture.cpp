#include "texture.h"
#include <iostream>
#include "../ThirdParty/stb_image.h"
#include <glad/glad.h>

void Texture2D::LoadFromPath(std::string textAssetPath)
{
	int width, height, nrChannels;
	stbi_uc *data;
	SAssetReader reader(textAssetPath);
	SPropertyValue sizeXData(reader.Get(), "TextureSizeX");
	SPropertyValue sizeYData(reader.Get(), "TextureSizeY");
	SPropertyValue channelCountData(reader.Get(), "TextureChannelsCount");
	SPropertyValue textureData(reader.Get(), "TextureData");

	if (!sizeXData.IsValid()) return;
	if (!sizeYData.IsValid()) return;
	if (!channelCountData.IsValid()) return;
	if (!textureData.IsValid()) return;

	width = *sizeXData.GetValue<int>();
	height = *sizeYData.GetValue<int>();
	nrChannels = *channelCountData.GetValue<int>();
	data = textureData.GetValue<stbi_uc>();

	GLenum format;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
}

Texture2D::Texture2D(std::string textAssetPath)
	: Asset(textAssetPath)
{
	LoadFromPath(textAssetPath);
}
