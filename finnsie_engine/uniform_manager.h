#ifndef UNIFORM_MANAGER_HG_
#define UNIFORM_MANAGER_HG_

#include <glad/glad.h>
#include <string>
#include <vector>

namespace finnsie {
	struct uniform 
	{
		std::string name;
		int location;

		uniform(std::string name, int shaderId)
		{
			this->name = name;
			this->location = glGetUniformLocation(shaderId, name.c_str());
		}
	};

	class UniformManager
	{
	public:
		// TODO(CK): Change to a map... need to retrieve by name and id
		std::vector<uniform*> uniforms;
		void CreateUniform(std::string name, int shaderId);
		int GetLocation(std::string name);
	};
}

#endif