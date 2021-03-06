#include "texture_manager.h"

#include <glad/glad.h>

namespace finnsie {

	void Generate(Texture2D* texture, unsigned int width, unsigned int height, unsigned int nrChannels, unsigned char* data)
	{
		texture->width = width;
		texture->height = height;

		switch (nrChannels)
		{
			case 1:
				texture->imageFormat = GL_ALPHA;
				texture->internalFormat = GL_ALPHA;
				break;
			case 3:
				texture->imageFormat = GL_RGB;
				texture->internalFormat = GL_RGB;
				break;
			case 4:
				texture->imageFormat = GL_RGBA;
				texture->internalFormat = GL_RGBA;
				break;
			default:
				texture->imageFormat = GL_RGB;
				texture->internalFormat = GL_RGB;
				break;
		}

		glBindTexture(GL_TEXTURE_2D, texture->id);
		glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, width, height,
			0, texture->imageFormat, GL_UNSIGNED_BYTE, data);
		// glGenerateMipmap(GL_TEXTURE_2D);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Set texture wrap and filter modes
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->filterMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->filterMax);

		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Bind(Texture2D* texture)
	{
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}
}