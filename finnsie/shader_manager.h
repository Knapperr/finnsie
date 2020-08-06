#ifndef SHADER_MANAGER_HG_
#define SHADER_MANAGER_HG_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace finnsie {
    
	struct uniform
	{
        // TODO(CK): Change to char*
		std::string name;
		int location;
		uniform(std::string name, int shaderId)
		{
			this->name = name;
			this->location = glGetUniformLocation(shaderId, name.c_str());
		}
	};
    
    struct Shader
    {
        unsigned int id;
        // TODO(CK): use raw array
        std::vector<uniform> uniforms;
        const char* vertexText;
        const char* fragmentText;
        const char* geometryText;
    };
    
    void GetUniforms(Shader* shader);
    void BuildShader(Shader* shader, const char* vertexFileName, 
                     const char* fragmentFileName, const char* geometryFileName = NULL);
    int GetLoc(Shader* shader, std::string name);
    void Reload(Shader* shader);
}

#endif