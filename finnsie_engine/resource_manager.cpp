#include "resource_manager.h"

#include <stb_image/stb_image.h>
#include <iostream>

namespace finnsie {
	
	Texture ResourceManager::GenerateTexture(int lookupId, const char* file, bool alpha)
	{
		textures[lookupId] = loadTextureFromFile(file, alpha);
		return textures[lookupId];
	}
	
	Shader ResourceManager::GenerateShader(int lookupId, const char* vertName, const char* fragName, const char* geoName)
	{
		Shader shader = {};
		BuildShader(&shader, vertName, fragName, geoName);
		shaders[lookupId] = shader;
		return shaders[lookupId];
	}

	Texture& ResourceManager::GetTexture(int lookupId)
	{
		return textures[lookupId];
	}

	Shader& ResourceManager::GetShader(int lookupId)
	{
		return shaders[lookupId];
	}

	void ResourceManager::ShutDown()
	{
		for (auto iter : shaders)
		{
			glDeleteProgram(iter.second.id);
		}

		for (auto iter : textures)
		{
			glDeleteTextures(1, &iter.second.id);
		}
	}

	Texture ResourceManager::loadTextureFromFile(const char* file, bool alpha)
	{
		// Init texture
		Texture texture = {};
		texture.internalFormat = GL_RGB;
		texture.imageFormat = GL_RGB;
		texture.wrapS = GL_REPEAT;
		texture.wrapT = GL_REPEAT;
		texture.filterMin = GL_LINEAR;
		texture.filterMax = GL_LINEAR;

		glGenTextures(1, &texture.id);

		if (alpha)
		{
			texture.internalFormat = GL_RGBA;
			texture.imageFormat = GL_RGBA;
		}

		int width, height, nrChannels;

		// TODO: Allow this to be configured
		unsigned char* image = stbi_load(file, &width, &height, &nrChannels, 0);
		if (image)
		{
			Generate(&texture, width, height, image);
		}
		else
		{
			std::cout << "TEXTURE::ERROR:: Failed  to load texture\n";
		}
		stbi_image_free(image);
		return texture;
	}

}