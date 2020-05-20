#ifndef UTILS_HG_
#define UTILS_HG_

#include <string>
#include "global.h"

namespace finnsie {
	// TODO(CK): Keep in here for now
	struct water_info
	{
		float uJump = 0.25f;
		float vJump = 0.25f;
		float tiling = 3.0f;
		float speed = 0.2f;
		float flowStrength = 0.07f;
		float flowOffset = -0.207f;
		float heightScale = 0.1f;
		float heightScaleModulated = 9.0f;
	};

	struct light_info
	{
		float lightX = -138.843f;
		float lightY = -19.247f;
		float lightZ = -14.226f;
	};

	struct draw_info
	{
		water_info waterInfo;
		light_info lightInfo;
	};

	unsigned int LoadTextureFile(const char* path, const std::string& directory, bool gamma);
	bool CreateEmptyModel();
	bool LoadEmptyModel(int index, std::string name, std::string path);
	bool LoadModel(std::string name, std::string path);
	bool LoadTexture(int index);
	bool LoadModels(std::string path);
	bool UnloadModel(std::string name);
	bool UnloadModel(int index);
	void LoadDistortedWater(Model &water);
	void LoadDirectionalWater(Model& water);
}
#endif