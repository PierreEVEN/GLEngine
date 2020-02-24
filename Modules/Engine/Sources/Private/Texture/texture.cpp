
#include <iostream>
#include <glad.h>
#include <stb_image.h>
#include <Texture/texture.h>

Texture2D::Texture2D(std::string textAssetPath)
	: Asset(textAssetPath) {
}

unsigned int Texture2D::GetTextureID()
{
	CheckData();
	if (AreDataLoaded())
	{
		return textureID;
	}
	else
	{
		return -1;
	}
}

void Texture2D::LoadProperties()
{
	SAssetReader reader(GetPath());
	SUIntPropertyValue* sizeXData = new SUIntPropertyValue(this, reader.Get(), "TextureSizeX");
	SUIntPropertyValue* sizeYData = new SUIntPropertyValue(this, reader.Get(), "TextureSizeY");
	SUIntPropertyValue* channelCountData = new SUIntPropertyValue(this, reader.Get(), "TextureChannelsCount");
	SPropertyValue* textureData = new SPropertyValue(this, reader.Get(), "TextureData");
	RegisterProperty(sizeXData);
	RegisterProperty(sizeYData);
	RegisterProperty(channelCountData);
	RegisterProperty(textureData);
	Asset::LoadProperties();
}

void Texture2D::PostLoadProperties()
{
	Asset::PostLoadProperties();
	SUIntPropertyValue* sizeXData = (SUIntPropertyValue*)GetProperty("TextureSizeX");
	SUIntPropertyValue* sizeYData = (SUIntPropertyValue*)GetProperty("TextureSizeY");
	SUIntPropertyValue* channelCountData = (SUIntPropertyValue*)GetProperty("TextureChannelsCount");
	SPropertyValue* textureData = GetProperty("TextureData");
	if (!sizeXData->IsValueValid() || !sizeYData->IsValueValid() || !channelCountData->IsValueValid() || !textureData->IsValueValid())
	{
		GFullLog(LogVerbosity::Assert, "Texture", "Failed to load texture " + GetName() + " data");
		return;
	}
	unsigned int width = sizeXData->GetUIntValue();
	unsigned int height = sizeYData->GetUIntValue();
	unsigned int nrChannels = channelCountData->GetUIntValue();
	stbi_uc* data = textureData->GetValue<stbi_uc>();
	GLenum format = GL_RED;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	glGenTextures(1, &textureID);
	//glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	//stbi_image_free(data);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
}

void Texture2D::BuildThumbnail()
{
	SPropertyValue* thumbnailProperty = GetBaseProperty("thumbnailTexture");
	SUIntPropertyValue* sizeXData = (SUIntPropertyValue*)GetProperty("TextureSizeX");
	SUIntPropertyValue* sizeYData = (SUIntPropertyValue*)GetProperty("TextureSizeY");
	SUIntPropertyValue* channelCountData = (SUIntPropertyValue*)GetProperty("TextureChannelsCount");
	GLenum format = GL_RGB;
	if (channelCountData->GetUIntValue() == 1)
		format = GL_RED;
	else if (channelCountData->GetUIntValue() == 3)
		format = GL_RGB;
	else if (channelCountData->GetUIntValue() == 4)
		format = GL_RGBA;
	unsigned int sizeX = sizeXData->GetUIntValue();
	unsigned int sizeY = sizeYData->GetUIntValue();
	unsigned int channelCount = channelCountData->GetUIntValue();
	unsigned char* renderTexture = (unsigned char*)malloc(sizeX * sizeY * channelCount * sizeof(unsigned char));
	glBindTexture(GL_TEXTURE_2D, GetTextureID());
	glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, renderTexture);
	float* finalTexture = (float*)malloc(THUMBNAIL_RESOLUTION * THUMBNAIL_RESOLUTION * 3 * sizeof(float));

	for (unsigned int pixelIndex = 0; pixelIndex < THUMBNAIL_RESOLUTION * THUMBNAIL_RESOLUTION; ++pixelIndex)
	{
		unsigned int posX = (unsigned int)((pixelIndex % THUMBNAIL_RESOLUTION) * (sizeX / (float)THUMBNAIL_RESOLUTION));
		unsigned int posY = (unsigned int)((pixelIndex / THUMBNAIL_RESOLUTION) * (sizeY / (float)THUMBNAIL_RESOLUTION));
		if (channelCount == 1)
		{
			finalTexture[pixelIndex * 3] = renderTexture[(posX + (posY * sizeX))] / 256.f;
			finalTexture[pixelIndex * 3 + 1] = renderTexture[(posX + (posY * sizeX))] / 256.f;
			finalTexture[pixelIndex * 3 + 2] = renderTexture[(posX + (posY * sizeX))] / 256.f;
		}
		else if (channelCount == 2)
		{
			finalTexture[pixelIndex * 3] = renderTexture[(posX + (posY * sizeX)) * 2] / 256.f;
			finalTexture[pixelIndex * 3 + 1] = renderTexture[(posX + (posY * sizeX)) * 2] / 256.f;
		}
		else
		{
			finalTexture[pixelIndex * 3] = renderTexture[(posX + (posY * sizeX)) * channelCount] / 256.f;
			finalTexture[pixelIndex * 3 + 1] = renderTexture[(posX + (posY * sizeX)) * channelCount + 1] / 256.f;
			finalTexture[pixelIndex * 3 + 2] = renderTexture[(posX + (posY * sizeX)) * channelCount + 2] / 256.f;
		}
	}
	thumbnailProperty->SetValue<float>(finalTexture, THUMBNAIL_RESOLUTION * THUMBNAIL_RESOLUTION * 3 * sizeof(float));
	free(renderTexture);
	free(finalTexture);
}

ImColor Texture2D::GetAssetColor()
{
	return ImColor(0.3f, 0.3f, 0.7f, 1.f);
}

void Texture2D::OnAssetClicked()
{
	//@Todo : new TextureEditorWindow(GetName(), this);
}

TextureCube::TextureCube(std::string textAssetPath)
	: Texture2D(textAssetPath) {}

unsigned int TextureCube::GetTextureID()
{
	CheckData(true);
	if (AreDataLoaded())
	{
		return textureCubeID;
	}
	else
	{
		return -1;
	}
}

void TextureCube::BuildThumbnail()
{

}

void TextureCube::LoadProperties()
{
	SAssetReader reader(GetPath());
	for (unsigned int i = 0; i < 6; ++i)
	{
		SUIntPropertyValue* sizeXData = new SUIntPropertyValue(this, reader.Get(), "TextureSizeX_" + std::to_string(i));
		SUIntPropertyValue* sizeYData = new SUIntPropertyValue(this, reader.Get(), "TextureSizeY_" + std::to_string(i));
		SUIntPropertyValue* channelCountData = new SUIntPropertyValue(this, reader.Get(), "TextureChannelsCount_" + std::to_string(i));
		SPropertyValue* textureData = new SPropertyValue(this, reader.Get(), "TextureData_" + std::to_string(i));
		RegisterProperty(sizeXData);
		RegisterProperty(sizeYData);
		RegisterProperty(channelCountData);
		RegisterProperty(textureData);
	}
	Asset::LoadProperties();
}

void TextureCube::PostLoadProperties()
{
	glGenTextures(1, &textureCubeID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubeID);

	for (unsigned int i = 0; i < 6; ++i)
	{
		int width, height, nrChannels;
		stbi_uc* data;
		SUIntPropertyValue* sizeXData = (SUIntPropertyValue*)GetProperty("TextureSizeX_" + std::to_string(i));
		SUIntPropertyValue* sizeYData = (SUIntPropertyValue*)GetProperty("TextureSizeY_" + std::to_string(i));
		SUIntPropertyValue* channelCountData = (SUIntPropertyValue*)GetProperty("TextureChannelsCount_" + std::to_string(i));
		SPropertyValue* textureData = GetProperty("TextureData_" + std::to_string(i));
		if (!sizeXData->IsValueValid() || !sizeYData->IsValueValid() || !channelCountData->IsValueValid() || !textureData->IsValueValid()) return;
		width = sizeXData->GetUIntValue();
		height = sizeYData->GetUIntValue();
		nrChannels = channelCountData->GetUIntValue();
		data = textureData->GetValue<stbi_uc>();
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
