#ifndef UTILS_HG_
#define UTILS_HG_

#include <string>
#include "global.h"

namespace finnsie {
	unsigned int LoadTextureFile(const char* path, const std::string& directory, bool gamma);
	bool CreateEmptyModel();
	bool LoadEmptyModel(int index, std::string name, std::string path);
	bool LoadModel(std::string name, std::string path);
	bool LoadTexture(int index);
	bool LoadModels(std::string path);
	bool UnloadModel(std::string name);
	bool UnloadModel(int index);
	void LoadDistortedWater(Model &water);
}
#endif