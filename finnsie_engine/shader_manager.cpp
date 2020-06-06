#include "shader_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "log.h"

namespace finnsie {

	Shader& Shader::UseShader(Shader* shader)
	{
		glUseProgram(shader->id);
		return *shader;
	}

	void Shader::BuildShader(Shader* shader, const char* vertexText, const char* fragmentText, const char* geometryText)
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

			//LoadUniforms(vertStream);
			LoadUniforms(fragStream);

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

		// TODO(CK): Keep the vShaderCode, fShaderCode and gShaderCode 
		// as members for "hot-reloading"

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

	// Get the uniforms for the shader
	void Shader::LoadUniforms(std::stringstream& shaderFile)
	{

		std::string lastWord;
		std::string temp;
		std::string type;
		
			// TODO(CK): don't read the whole entire file
			// once uniforms are done then exit this...
		
		while (shaderFile >> temp)
		{
			// after the type has been set we can grab the name
			if (type != "")
			{
				if (type == "int")
				{
					uniforms.push_back(Uniform(1.0, temp, type));
				}
				if (type == "float")
				{
					uniforms.push_back(FloatUniform(false, temp, type));

				}
				if (type == "bool")
				{
					uniforms.push_back(BoolUniform(1.0f, temp, type));

				}
				type = "";
			}

			// after getting uniform set the type
			if (lastWord == "uniform")
			{
				type = temp;
				lastWord = "";
			}
			
			if (temp == "uniform")
			{
				lastWord = temp;
			}
		}
	}

	// Check shader compilation/linking errors
	void Shader::CheckCompileErrors(unsigned int shader, std::string type)
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
}