#include "shader_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>


Shader& UseShader(Shader* shader)
{
	glUseProgram(shader->id);
	return *shader;
}

// TODO: Read google keep note look at resource_manager it gives each shader a name after building
void BuildShaders(Shader* shader, const char* vertexText, const char* fragmentText, const char* geometryText)
{
	std::string vertCode;
	std::string fragCode;
	std::string geoCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexText);
		fShaderFile.open(fragmentText);
		std::stringstream vertStream, fragStream;

		vertStream << vShaderFile.rdbuf();
		fragStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		// Convert streams to strings
		vertCode = vertStream.str();
		fragCode = fragStream.str();

		// Geometry shader is optional
		if (geometryText != NULL)
		{
			gShaderFile.open(geometryText);
			std::stringstream geoStream;
			geoStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geoCode = geoStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Error::SHADER: File not successfully read\n";
	}

	const char* vShaderCode = vertCode.c_str();
	const char* fShaderCode = fragCode.c_str();

	// Compile the shaders
	unsigned int vertex, fragment;
	// Vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");

	unsigned int geometry;
	if (geometryText != nullptr)
	{
		const char* gShaderCode = geoCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		CheckCompileErrors(geometry, "GEOMETRY");
	}
	// Shader program
	shader->id = glCreateProgram();
	glAttachShader(shader->id, vertex);
	glAttachShader(shader->id, fragment);
	if (geometryText != nullptr)
	{
		glAttachShader(shader->id, geometry);
	}
	glLinkProgram(shader->id);
	CheckCompileErrors(shader->id, "PROGRAM");
	// Delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryText != nullptr)
	{
		glDeleteShader(geometry);
	}
}

// Check shader compilation/linking errors
void CheckCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------\n";
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- ---------------------------------------------\n";
		}
	}
}