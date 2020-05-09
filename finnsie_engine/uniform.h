#ifndef UNIFORM_HG_
#define UNIFORM_HG_

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
		std::vector<uniform*> uniforms;
		void CreateUniform(std::string name, int shaderId);
		int GetLocation(std::string name);
	};
}

#endif