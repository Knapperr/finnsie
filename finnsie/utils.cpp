#include "utils.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "log.h"

namespace finnsie {

	std::vector<GameObject*> g_objects;

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
		g_objects.push_back(obj);
		return true;
	}

	bool LoadEmptyObject(int index, std::string name, std::string path, float x, float y, float z)
	{
		g_objects[index]->name = name;
		g_objects[index]->pos= glm::vec3(x, y, z),
		g_objects[index]->orientation = glm::vec3(0.0f, 0.0f, 0.0f),
		g_objects[index]->scale = 10.0f,
		g_objects[index]->model = new Model(path);

		if (g_objects[index]->model == NULL)
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
		g_objects[index]->model->meshes[0].textures.push_back(uvtexture);
		g_objects[index]->model->meshes[0].textures.push_back(flowtexture);
		g_objects[index]->model->meshes[0].textures.push_back(normaltexture);
		return true;
	}

	void LoadDistortedWater(Model* water)
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

		water->meshes[0].textures.push_back(uvtexture);
		water->meshes[0].textures.push_back(flowtexture);
		water->meshes[0].textures.push_back(normaltexture);
	}

	void LoadDirectionalWater(Model* water)
	{
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

		water->meshes[0].textures.push_back(uvtexture);
		water->meshes[0].textures.push_back(flowtexture);
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
		for (unsigned int i = 0; i < g_objects.size(); ++i)
		{
			if (g_objects[i]->name == name)
			{
				obj = g_objects[i];
				found = true;
				break;
			}
		}
		if (!found) { return false; }

		g_objects.erase(std::remove(g_objects.begin(), g_objects.end(), obj), g_objects.end());
		delete obj;
		return true;
	}

	bool UnloadObject(int index)
	{
		delete g_objects[index];
		g_objects.erase(std::remove(g_objects.begin(), g_objects.end(), g_objects[index]), g_objects.end());
		return true;
	}

}
