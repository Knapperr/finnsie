#ifndef SHADER_MANAGER_HG_
#define SHADER_MANAGER_HG_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

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
    
	class Shader
	{
        public:
		unsigned int id;
		std::vector<uniform> uniforms;
        
		Shader& UseShader(Shader* shader);
		void GetUniforms();
		int GetLoc(std::string name);
		void BuildShader(Shader* shader, const char* vertexText, const char* fragmentText, const char* geometryText = NULL);
		void Reload();
        
        private:
		void checkCompileErrors(unsigned int shader, std::string type);
        
	};
}

#endif