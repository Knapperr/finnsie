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
		checkCompileErrors(vertex, "VERTEX");

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		unsigned int geometry;
		if (geometryText != nullptr)
		{
			const char* gShaderCode = geoCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
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
		checkCompileErrors(shader->id, "PROGRAM");
		// Delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryText != nullptr)
		{
			glDeleteShader(geometry);
		}
	}

	// Load the uniforms from the shader file
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
					uniforms.push_back(Uniform(1.0, temp, "slider"));
				}
				if (type == "float")
				{
					uniforms.push_back(FloatUniform(false, temp, "slider"));

				}
				if (type == "bool")
				{
					uniforms.push_back(BoolUniform(1.0f, temp, "checkbox"));

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

	// Get the uniform
	void Shader::ActivateUniforms()
	{
		// This should work because the "drawInfo" is just the state of that uniforms value
		// so if we use this method it should be fine its just using the shader itself
		if (uniforms[0].guiType == "slider")
		{
			glUniform1f(glGetUniformLocation(this->id, uniforms[0].name.c_str()), uniforms[0].value);
		}

		for (int i = 0; i < uniforms.size(); ++i)
		{
			// vec3s
			//glUniform3fv(glGetUniformLocation(waterShader.id, "lightPos"), 1, &lightPos[0]);
			//glUniform3fv(glGetUniformLocation(waterShader.id, "viewPos"), 1, &camPos[0]); // getting updated in BeginRender (probably not good)

			// floats
			//glUniform1f(this->uniformManager->GetLocation("uJump"), drawInfo.waterInfo.uJump);
			//glUniform1f(this->uniformManager->GetLocation("vJump"), drawInfo.waterInfo.vJump);
			//glUniform1f(this->uniformManager->GetLocation("tiling"), drawInfo.waterInfo.tiling);
			//glUniform1f(this->uniformManager->GetLocation("speed"), drawInfo.waterInfo.speed);
			//glUniform1f(this->uniformManager->GetLocation("flowStrength"), drawInfo.waterInfo.flowStrength);
			//glUniform1f(this->uniformManager->GetLocation("flowOffset"), drawInfo.waterInfo.flowOffset);
			//glUniform1f(this->uniformManager->GetLocation("heightScale"), drawInfo.waterInfo.heightScale);
			//glUniform1f(this->uniformManager->GetLocation("heightScaleModulated"), drawInfo.waterInfo.heightScaleModulated);

			// this is a bool but it works?
			//glUniform1f(this->uniformManager->GetLocation("dualGrid"), drawInfo.waterInfo.dualGrid);

		}
	}

	// Check shader compilation/linking errors
	void Shader::checkCompileErrors(unsigned int shader, std::string type)
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