#ifndef UTILS_HG_
#define UTILS_HG_

#include <string>

namespace finnsie {
	bool LoadModel(std::string name, std::string path);
	bool LoadModels();
	bool UnloadModel(std::string name);
}
#endif