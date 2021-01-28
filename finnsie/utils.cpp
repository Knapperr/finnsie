#include "utils.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "log.h"
#include "game.h"

namespace finnsie {

	Game* g_Game;

	// TODO(CK): TEMP FUNCTION USE RESOURCE MANAGER (Create texture with resource manager and pass it to the function)
	unsigned int LoadTextureFile(const char* path, const std::string& directory, bool gamma = false)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

		if (data)
		{
			GLenum format = GL_RGB;
			if (nrComponents == 1)
				format = GL_ALPHA;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;
			
			//GLenum format;
			//if (nrComponents == 1)
			//	format = GL_RED;
			//else if (nrComponents == 3)
			//	format = GL_RGB;
			//else if (nrComponents == 4)
			//	format = GL_RGBA;

				
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// unbind after using
			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(data);
		}
		else
		{
			std::string msg = "Texture failed to load at path: " + (std::string)path;
			LOG_WARN(msg.c_str());
			stbi_image_free(data);
		}
		return textureID;
	}

	bool CreateEmptyObject()
	{
		GameObject* obj = new GameObject();
		g_Game->objects.push_back(obj);
		return true;
	}

	bool LoadEmptyObject(int index, std::string name, std::string path, float x, float y, float z)
	{
		g_Game->objects[index]->name = name;
		g_Game->objects[index]->pos= glm::vec3(x, y, z),
		g_Game->objects[index]->orientation = glm::vec3(0.0f, 0.0f, 0.0f),
		g_Game->objects[index]->scale = 10.0f,
		g_Game->objects[index]->model = new Model(path);

		if (g_Game->objects[index]->model == NULL)
			return false;

		return true;
	}

	bool LoadTexture(int index)  
	{
		// CREATE A BASIC SHAPE LOADER replace ASSIMP
		// WE WILL HAVE TO PUSH THIS TEXTURE TO THE TEXUTES OF THE QUAD 
		// IN model.h
		std::string uvpath = "content/textures/water/water.png";
		std::string uvdirectory = uvpath.substr(0, uvpath.find_last_of('/'));
		Texture uvtexture = {};
		uvtexture.id = LoadTextureFile("water.png", uvdirectory);
		uvtexture.type = "texture_diffuse";
		uvtexture.path = uvpath;
		
		std::string flowpath = "content/textures/water/flow-speed-noise.png";
		std::string flowdirectory = flowpath.substr(0, flowpath.find_last_of('/'));
		Texture flowtexture = {};
		flowtexture.id = LoadTextureFile("flow-speed-noise.png", flowdirectory);
		flowtexture.type = "texture_normal";
		flowtexture.path = flowpath;

		std::string normalpath = "content/textures/water/water-derivative-height.png";
		std::string normaldir = normalpath.substr(0, normalpath.find_last_of('/'));
		Texture normaltexture = {};
		normaltexture.id = LoadTextureFile("water-derivative-height.png", normaldir);
		normaltexture.type = "texture_normal";
		normaltexture.path = normalpath;

		// NOTE(CK): For now only using this for primitives (can use mesh index 0)
		g_Game->objects[index]->model->meshes[0].textures.push_back(uvtexture);
		g_Game->objects[index]->model->meshes[0].textures.push_back(flowtexture);
		g_Game->objects[index]->model->meshes[0].textures.push_back(normaltexture);
		return true;
	}

	void LoadDistortedWater(WaterObject* water)
	{
		water->tiling = 5.0f;
		water->speed = 0.3f;
		water->flowStrength = 0.05f;
		water->flowOffset = -0.23f;
		water->heightScale = 0.1f;
		water->heightScaleModulated = 8.0f;

		// CREATE A BASIC SHAPE LOADER replace ASSIMP
		// WE WILL HAVE TO PUSH THIS TEXTURE TO THE TEXUTES OF THE QUAD 
		// IN model.h
		std::string uvpath = "content/textures/water/water.png";
		std::string uvdirectory = uvpath.substr(0, uvpath.find_last_of('/'));
		Texture uvtexture = {};
		uvtexture.id = LoadTextureFile("water.png", uvdirectory);
		uvtexture.type = "texture_diffuse";
		uvtexture.path = uvpath;

		std::string flowpath = "content/textures/water/flow-speed-noise.png";
		std::string flowdirectory = flowpath.substr(0, flowpath.find_last_of('/'));
		Texture flowtexture = {};
		flowtexture.id = LoadTextureFile("flow-speed-noise.png", flowdirectory);
		flowtexture.type = "texture_normal";
		flowtexture.path = flowpath;

		std::string normalpath = "content/textures/water/water-derivative-height.png";
		std::string normaldir = normalpath.substr(0, normalpath.find_last_of('/'));
		Texture normaltexture = {};
		normaltexture.id = LoadTextureFile("water-derivative-height.png", normaldir);
		normaltexture.type = "texture_normal";
		normaltexture.path = normalpath;

		water->model->meshes[0].textures.push_back(uvtexture);
		water->model->meshes[0].textures.push_back(flowtexture);
		water->model->meshes[0].textures.push_back(normaltexture);
	}

	void LoadDirectionalWater(WaterObject* water)
	{
		water->tiling = 10.0f;
		water->speed = 0.2f;
		water->flowStrength = 0.07f;
		water->heightScale = 0.92f;
		water->heightScaleModulated = 9.0f;
		water->gridResolution = 30.0f;
		water->tilingModulated = 50.0f;
		water->dualGrid = true;

		std::string uvpath = "content/textures/water/ripples-derivative-height.png";
		std::string uvdirectory = uvpath.substr(0, uvpath.find_last_of('/'));
		Texture uvtexture = {};
		uvtexture.id = LoadTextureFile("ripples-derivative-height.png", uvdirectory);
		uvtexture.type = "texture_diffuse";
		uvtexture.path = uvpath;

		std::string flowpath = "content/textures/water/flow-speed-noise.png";
		std::string flowdirectory = flowpath.substr(0, flowpath.find_last_of('/'));
		Texture flowtexture = {};
		flowtexture.id = LoadTextureFile("flow-speed-noise.png", flowdirectory);
		flowtexture.type = "texture_normal";
		flowtexture.path = flowpath;

		water->model->meshes[0].textures.push_back(uvtexture);
		water->model->meshes[0].textures.push_back(flowtexture);
	}

	/* NOTE(CK): OG (USE THIS FOR LOADING TEXTURES FROM GUI )
	bool LoadTexture(int index)
	{
		// CREATE A BASIC SHAPE LOADER replace ASSIMP
		// WE WILL HAVE TO PUSH THIS TEXTURE TO THE TEXUTES OF THE QUAD 
		// IN model.h
		std::string path = "content/textures/water/uv.png";
		std::string directory = path.substr(0, path.find_last_of('/'));
		Texture texture = {};
		texture.id = LoadTextureFile("uv.png", directory);
		texture.type = "texture_diffuse";
		texture.path = path;

		// NOTE(CK): For now only using this for primitives (can use mesh index 0)
		g_models[index]->meshes[0].textures.push_back(texture);
		return true;
	}
	*/

	bool UnloadObject(std::string name)
	{
		bool found = false;
		GameObject* obj;
		for (unsigned int i = 0; i < g_Game->objects.size(); ++i)
		{
			if (g_Game->objects[i]->name == name)
			{
				obj = g_Game->objects[i];
				found = true;
				break;
			}
		}
		if (!found) { return false; }

		g_Game->objects.erase(std::remove(g_Game->objects.begin(), g_Game->objects.end(), obj), g_Game->objects.end());
		delete obj;
		return true;
	}

	bool UnloadObject(int index)
	{
		delete g_Game->objects[index];
		g_Game->objects.erase(std::remove(g_Game->objects.begin(), g_Game->objects.end(), g_Game->objects[index]), g_Game->objects.end());
		g_Game->currentObject = nullptr;
		return true;
	}

}
