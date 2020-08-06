#ifndef UTILS_HG_
#define UTILS_HG_

#include <string>
#include "global.h"

namespace finnsie {
	unsigned int LoadTextureFile(const char* path, const std::string& directory, bool gamma);
	bool CreateEmptyObject();
	bool LoadEmptyObject(int index, std::string name, std::string path);
	bool LoadTexture(int index);
	bool UnloadObject(std::string name);
	bool UnloadObject(int index);
	void LoadDistortedWater(Model* water);
	void LoadDirectionalWater(Model* water);
}
#endif