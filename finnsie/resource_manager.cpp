#include "resource_manager.h"

#include <stb_image/stb_image.h>
#include <iostream>

#include "log.h"

namespace finnsie {
	
	// Init static variables
	std::unordered_map <int, Texture2D> ResourceManager::textures;
	std::unordered_map <std::string, Shader> ResourceManager::shaders;

	void ResourceManager::Init()
	{
		// TODO(CK):
		/*
			Read through a resource file that 
			has shader name and file names in it
		
			also has texture information in it as well

			after this information has been collected you
			generate all the shaders and textures in the 
			file into the resource manager

		*/

		GenerateShader("model",
					"shaders/model_vert.glsl",
					"shaders/model_frag.glsl",
					NULL);

		GenerateShader("normal",
					"shaders/onlynormals_model_vert.glsl",
					"shaders/onlynormals_model_frag.glsl",
					"shaders/onlynormals_model_geo.glsl");

		GenerateShader("waterDis",
					"shaders/waterdistortion_vert.glsl",
					"shaders/waterdistortion_frag.glsl",
					NULL);

		GenerateShader("waterDir",
					"shaders/waterdirection_vert.glsl",
					"shaders/waterdirection_frag.glsl",
					NULL);

		GenerateShader("binn",
					"shaders/blinnphong_vert.glsl",
					"shaders/blinnphong_frag.glsl",
					NULL);

		GenerateShader("cubemap",
					"shaders/cubemap_vert.glsl",
					"shaders/cubemap_frag.glsl",
					NULL);

		GenerateShader("grass",
					"shaders/grass_vert.glsl",
					"shaders/grass_frag.glsl",
					NULL);

		GenerateShader("light",
					   "shaders/light_vert.glsl",
					   "shaders/light_frag.glsl",
					   NULL);
	}

	Texture2D ResourceManager::GenerateTexture(int lookupId, const char* file, bool alpha)
	{
		textures[lookupId] = loadTextureFromFile(file, alpha);
		return textures[lookupId];
	}
	
	Shader ResourceManager::GenerateShader(std::string lookup, const char* vertName, const char* fragName, const char* geoName)
	{
		Shader shader = {};
		BuildShader(&shader, lookup, vertName, fragName, geoName);
		shaders[lookup] = shader;
		return shaders[lookup];
	}
    
	Texture2D* ResourceManager::GetTexture(int lookupId)
	{
		return &textures[lookupId];
	}
    
	Shader* ResourceManager::GetShader(std::string lookup)
	{
		Shader *shader = &shaders[lookup];
		assert(shader->name == lookup);
		return shader;	
	}
    
	int ResourceManager::GetShaderCount()
	{
		return shaders.size();
	}

	void ResourceManager::Shutdown()
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
    
	Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
	{
		// Init texture
		Texture2D texture = {};
		texture.internalFormat = GL_RGB;
		texture.imageFormat = GL_RGB;
		texture.wrapS = GL_REPEAT;
		texture.wrapT = GL_REPEAT;
		texture.filterMin = GL_LINEAR_MIPMAP_LINEAR;
		texture.filterMax = GL_LINEAR;
        
		glGenTextures(1, &texture.id);
        
		int width, height, nrChannels;
        
		// TODO: Allow this to be configured
		unsigned char* image = stbi_load(file, &width, &height, &nrChannels, 0);
		if (image)
		{
			Generate(&texture, width, height, nrChannels, image);
		}
		else
		{
			std::cout << "TEXTURE::ERROR Failed  to load texture\n";
		}
		stbi_image_free(image);
		return texture;
	}
    
}