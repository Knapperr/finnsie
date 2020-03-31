#include "utils.h"
#include "global.h"
#include "model.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace finnsie {

	std::vector<Model*> g_models;
	
	bool LoadModel(std::string name, std::string path)
	{
		// need to check if the model is loaded already
		for (int i = 0; i < g_models.size(); ++i)
		{
			if (g_models[i]->modelName == name)
			{
				std::cout << name << " is already loaded\n";
				return false;
			}
		}
		// TODO(CK): Don't really like loading the model with the constructor
		Model* model = new Model(name,
								false,
								glm::vec3(-100.0f, -30.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f),
								10.0f,
								path);
		g_models.push_back(model);
		return true;
	}

	bool LoadModels()
	{
		// TODO(CK): Change to bool
		/* TODO(CK): Read a file with all of this data in it. 
					 An editor would just use one big file
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

		//Model* treasureModel = new Model("teller",
		//								 false,
		//								 glm::vec3(-100.0f, -30.0f, 0.0f),
		//								 glm::vec3(0.0f, 0.0f, 0.0f),
		//								 1.0f,
		//								 "content/objects/treasureteller/TradeMan.obj");
		//g_models.push_back(treasureModel);

		//Model* battlefield = new Model("battle",
		//							   false,
		//							   glm::vec3(-200.0f, -10.0f, 0.0f),
		//							   glm::vec3(0.0f, 0.0f, 0.0f),
		//							   10.0f,
		//							   "content/objects/bombbattlefield/Bob-omb Battlefield.obj");
		//g_models.push_back(battlefield);
		
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
	}

	bool UnloadModel(std::string name)
	{
		bool found = false;
		Model* model;
		for (int i = 0; i < g_models.size(); ++i)
		{
			if (g_models[i]->modelName == name)
			{
				model = g_models[i];
				found = true;
				break;
			}
		}
		if (!found) { return false; }

		g_models.erase(std::remove(g_models.begin(), g_models.end(), model), g_models.end());
		return true;
	}
}
