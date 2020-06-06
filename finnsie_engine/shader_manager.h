#ifndef SHADER_MANAGER_HG_
#define SHADER_MANAGER_HG_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "uniform.h"


namespace finnsie {

	class Shader
	{
	public:
		unsigned int id;
		std::vector<Uniform> uniforms;

		Shader& UseShader(Shader* shader);
		void LoadUniforms(std::stringstream& shaderFile);
		void BuildShader(Shader* shader, const char* vertexText, const char* fragmentText, const char* geometryText = NULL);
		void CheckCompileErrors(unsigned int shader, std::string type);

	};

	
	
	

}

#endif