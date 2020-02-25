#ifndef OBJLOADER_HG_
#define OBJLOADER_HG_

#include "global.h"
#include <vector>

namespace finnsie {
	class ObjLoader 
	{
	public:
		bool Load(const char* path, std::vector<glm::vec3>& out_vertcies, 
				  std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals);
	};
}

#endif