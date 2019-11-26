#ifndef SHADER_MANAGER_HG_
#define SHADER_MANAGER_HG_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

struct Shader
{
	unsigned int id;
};

Shader& UseShader(Shader* shader);
void BuildShaders(Shader* shader, const char* vertexText, const char* fragmentText, const char* geometryText = NULL);
void CheckCompileErrors(unsigned int shader, std::string type);

#endif