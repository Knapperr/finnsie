#include "uniform.h"

namespace finnsie {

	void UniformManager::CreateUniform(std::string name, int shaderId)
	{
		uniforms.push_back(new uniform(name, shaderId));
	}

	// TODO(CK): Need a better solution to this. Probably more 
	// expensive doing this than just calling GlGetUniformLocation
	// since we always know which locations we need could just hold 
	// a list of ints
	int UniformManager::GetLocation(std::string name)
	{
		for (int i = 0; i != uniforms.size(); ++i)
		{
			if (uniforms[i]->name == name)
			{
				return uniforms[i]->location;
			}
		}
		return -1;
	}
}