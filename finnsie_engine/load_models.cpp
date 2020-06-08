#include "utils.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "log.h"

namespace finnsie {

	std::vector<Model*> g_models;

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

	bool CreateEmptyModel()
	{
		Model* model = new Model();
		model->drawInfo->modelName = "empty";
		g_models.push_back(model);
		return true;
	}

	
	bool LoadEmptyModel(int index, std::string name, std::string path)
	{
		// Delete the memory and then erase it from the vector 
		// avoid dangling pointer

		// TODO(CK): instead of doing this we should just create an empty model then 
		// run the load mesh function on it that will be part of the model class
		// still have to figure out how to load up a different mesh

		// UPDATE(MAY 14): Might be better to just do it this way in case the mesh is changed
		/* THIS IMPLEMENTATION WON'T WORK
		delete g_models[index];
		g_models.erase(std::remove(g_models.begin(), g_models.end(), g_models[index]), g_models.end());

		g_models.push_back(new Model(name,
									false,
									glm::vec3(-100.0f, -30.0f, 0.0f),
									glm::vec3(0.0f, 0.0f, 0.0f),
									10.0f,
									path));
		*/
		delete g_models[index];
		
		g_models[index] = new Model(name,
									glm::vec3(-100.0f, -30.0f, 0.0f),
									glm::vec3(0.0f, 0.0f, 0.0f),
									10.0f,
									path);
		return true;
	}

	/*
	bool LoadModel(std::string name, std::string path)
	{
		// need to check if the model is loaded already
		for (int i = 0; i < g_models.size(); ++i)
		{
			if (g_models[i]->drawInfo->modelName == name)
			{
				std::string msg = name + " is already loaded\n";
				LOG_WARN(msg.c_str());
				return false;
			}
		}
		// TODO(CK): Don't really like loading the model with the constructor
		Model* model = new Model(false,
								glm::vec3(-100.0f, -30.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f),
								10.0f,
								path);
		g_models.push_back(model);
		return true;
	}
	*/

	// TODO(CK): Temp function for loading all of the texture to the quad
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
		g_models[index]->meshes[0].textures.push_back(uvtexture);
		g_models[index]->meshes[0].textures.push_back(flowtexture);
		g_models[index]->meshes[0].textures.push_back(normaltexture);
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

	bool LoadModels(std::string path)
	{
		/* TODO(CK): Read a file with all of this data in it. 
					 An editor would just use one big file??? maybe not
		name: 
		wireFrame: 1 or 0
		pos: -20.0f, -3.75f, 0.0f
		orientation: 0.0f, 0.0f, 0.0f
		scale:
		location:

		reads pushes back until end of file
		file structure needs to match or the loop will be wrong
		could do one quick search couting name but that wouldnt be efficient
		*/
        
        /*
        char textBuffer[256];
        _snprintf_s(textBuffer, sizeof(textBuffer),
                    "LPC:%u BTL: %u TC:%u BTW:%u - PC:%u WC:%u\n", lastPlayCursor, byteToLock, targetCursor, bytesToWrite, playCursor, writeCursor);
        OutputDebugStringA(textBuffer);*/
        
		//const char* fileName = "name";
		//std::ifstream assetFile(fileName);
		//if (!assetFile.is_open())
		//{
		//	return false;
		//}

		// for loop to grab data

		//float num = 0;
		//while (verticesFile >> num)
		//{
		//	this->vertices.push_back(num);
		//}

		//verticesFile.close();
		//return true;

		//Model* ourModel = new Model("donut",
		//							true,
		//							glm::vec3(-20.0f, -3.75f, 0.0f),
		//							glm::vec3(0.0f, 0.0f, 0.0f),
		//							1.0f,
		//							"content/objects/donuttext/donutscaletext.obj");
		//g_models.push_back(ourModel);

		//Model* nanoModel = new Model("nano",
		//							 false,
		//							 glm::vec3(-40.0f, -5.75f, 0.0f),
		//							 glm::vec3(0.0f, 2.0f, 0.0f),
		//							 0.2f,
		//							 "content/objects/nanosuit/nanosuit.obj");
		//g_models.push_back(nanoModel);

		//Model* rockModel = new Model("rock",
		//							 false,
		//							 glm::vec3(-60.0f, -5.75f, 0.0f),
		//							 glm::vec3(0.0f, 0.0f, 0.0f),
		//							 0.5f,
		//							 "content/objects/column/OldColumn.obj");
		//g_models.push_back(rockModel);

		//Model* eraserModel = new Model("eraser",
		//							   false,
		//							   glm::vec3(-78.0f, -5.75f, 0.0f),
		//							   glm::vec3(0.0f, 0.0f, 0.0f),
		//							   1.0f,
		//							   "content/objects/eraser/eraser.obj");
		//g_models.push_back(eraserModel);

		//Model* mansionModel = new Model("mansion",
		//								false,
		//								glm::vec3(-300.0f, -50.0f, 0.0f),
		//								glm::vec3(0.0f, 0.0f, 0.0f),
		//								0.02f,
		//								"content/objects/mansion/op_mansion.obj");
		//g_models.push_back(mansionModel);

		/*
		Model* battlefield = new Model("battle",
									   false,
									   glm::vec3(-200.0f, -10.0f, 0.0f),
									   glm::vec3(0.0f, 0.0f, 0.0f),
									   10.0f,
									   "content/objects/bombbattlefield/Bob-omb Battlefield.obj");
		g_models.push_back(battlefield);

		Model* treasureModel = new Model("teller",
										 false,
										 glm::vec3(-100.0f, -30.0f, 0.0f),
										 glm::vec3(0.0f, 0.0f, 0.0f),
										 1.0f,
										 "content/objects/treasureteller/TradeMan.obj");
		g_models.push_back(treasureModel);
		
		// DOESN'T WORK
		//Model* outsetModel = new Model("outset",
		//								 false,
		//								 glm::vec3(-100.0f, -30.0f, 0.0f),
		//								 glm::vec3(0.0f, 0.0f, 0.0f),
		//								 1.0f,
		//								 "content/objects/outset/Island.obj");
		//g_models.push_back(outsetModel);

		//Model* windfall = new Model("outset",
		//							false,
		//							glm::vec3(-100.0f, -30.0f, 0.0f),
		//							glm::vec3(0.0f, 0.0f, 0.0f),
		//							0.05f,
		//							"content/objects/windfall/Windfall.obj");
		//g_models.push_back(windfall);



		return true;
		*/
		return false;
	}

	bool UnloadModel(std::string name)
	{
		bool found = false;
		Model* model;
		for (int i = 0; i < g_models.size(); ++i)
		{
			if (g_models[i]->drawInfo->modelName == name)
			{
				model = g_models[i];
				found = true;
				break;
			}
		}
		if (!found) { return false; }

		g_models.erase(std::remove(g_models.begin(), g_models.end(), model), g_models.end());
		delete model;
		return true;
	}

	bool UnloadModel(int index)
	{
		delete g_models[index];
		g_models.erase(std::remove(g_models.begin(), g_models.end(), g_models[index]), g_models.end());
		return true;
	}
}
