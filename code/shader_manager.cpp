#include "shader_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "log.h"

namespace finnsie {
    
	Shader& UseShader(Shader* shader)
	{
		glUseProgram(shader->id);
		return *shader;
	}
    
    
	// Check shader compilation/linking errors
	void checkCompileErrors(unsigned int shader, std::string type)
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
    
	/*
		You can do full reflection on shader with glGet
		names, types, uniforms, attributes, samplers, everything

		you could use uniforms blocks/buffers and then share same struct between C code and GLSL code
		then do glMapBuffer for uniform buffer and memcpy all the data, to update it on GPU
	*/
	void GetUniforms(Shader* shader)
	{
        // TODO(CK): Definitely switch to an array of uniforms this 
        // is not efficient or worth it 
        if (!shader->uniforms.empty())
        {
            shader->uniforms.clear();
        }
        
		int count;
		glGetProgramiv(shader->id, GL_ACTIVE_UNIFORMS, &count);
		
		int size;
		unsigned int type;
		const int bufSize = 28;
		char name[bufSize];
		int nameLength;
        
		for (int i = 0; i < count; i++)
		{
			glGetActiveUniform(shader->id, (GLuint)i, bufSize, &nameLength, &size, &type, name);
			shader->uniforms.push_back(uniform(name, shader->id));
		}
	}
    
	void BuildShader(Shader* shader, const char* vertexFileName, 
                     const char* fragmentFileName, const char* geometryFileName)
	{
        // Set file names
        shader->vertexText = vertexFileName;
        shader->fragmentText = fragmentFileName;
        shader->geometryText = geometryFileName;
        
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
			vShaderFile.open(shader->vertexText);
			fShaderFile.open(shader->fragmentText);
			std::stringstream vertStream, fragStream;
            
			vertStream << vShaderFile.rdbuf();
			fragStream << fShaderFile.rdbuf();
            
			vShaderFile.close();
			fShaderFile.close();
            
			// Convert streams to strings
			vertCode = vertStream.str();
			fragCode = fragStream.str();
            
			// Geometry shader is optional
			if (geometryFileName != NULL)
			{
				gShaderFile.open(shader->geometryText);
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
		checkCompileErrors(vertex, "VERTEX");
        
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
        
		unsigned int geometry;
		if (geometryFileName != NULL)
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
		if (geometryFileName != NULL)
		{
			glAttachShader(shader->id, geometry);
		}
		glLinkProgram(shader->id);
		checkCompileErrors(shader->id, "PROGRAM");
		// Delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryFileName != NULL)
		{
			glDeleteShader(geometry);
		}
        
		GetUniforms(shader);
	}
    
	// TODO(CK): Need a better solution to this. Probably more 
	// expensive doing this than just calling GlGetUniformLocation
	// since we always know which locations we need could just hold 
	// a list of ints
	int GetLoc(Shader* shader, std::string name)
	{
		for (int i = 0; i != shader->uniforms.size(); ++i)
		{
			if (shader->uniforms[i].name == name)
			{
				return shader->uniforms[i].location;
			}
		}
        // Not found
		return -1;
	}
    
    // TODO(CK): Create a function that does this
    // so that the above code can call it as well
    // compileShaders()
    void Reload(Shader* shader)
    {
        // TODO(CK): Check to see if file has been changed
        // have to keep file attributes when first reading it
        // in build shader can return a boolean from this routine
        
        
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
			vShaderFile.open(shader->vertexText);
			fShaderFile.open(shader->fragmentText);
			std::stringstream vertStream, fragStream;
            
			vertStream << vShaderFile.rdbuf();
			fragStream << fShaderFile.rdbuf();
            
			vShaderFile.close();
			fShaderFile.close();
            
			// Convert streams to strings
			vertCode = vertStream.str();
			fragCode = fragStream.str();
            
			// Geometry shader is optional
			if (shader->geometryText != NULL)
			{
				gShaderFile.open(shader->geometryText);
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
		checkCompileErrors(vertex, "VERTEX");
        
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
        
		unsigned int geometry;
		if (shader->geometryText != NULL)
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
		if (shader->geometryText != NULL)
		{
			glAttachShader(shader->id, geometry);
		}
		glLinkProgram(shader->id);
		checkCompileErrors(shader->id, "PROGRAM");
		// Delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (shader->geometryText != NULL)
		{
			glDeleteShader(geometry);
		}
        
		GetUniforms(shader);
        
        return;
    }
}