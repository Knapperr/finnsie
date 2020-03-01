#include "primitive_model.h"

#include <iostream>
#include <fstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>

namespace finnsie {

	void PrimitiveModel::LoadTexture(ModelTexture& texture, unsigned int shaderId, const char* fileLocation, const char* uniformName, GLint value)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(fileLocation, &width, &height, &nrChannels, 0);
		stbi_set_flip_vertically_on_load(true);
		if (data)
		{
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glGenTextures(1, &texture.id);
			glBindTexture(GL_TEXTURE_2D, texture.id);

			// Set wrapping params
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture.wrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture.wrapT);

			// Set filtering params
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture.minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture.magFilter);

			glTexImage2D(GL_TEXTURE_2D, 0, texture.internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// Need to tell the shader where the texture belongs (which is this texture)
			// not sure if i need to do this here
			glUseProgram(shaderId);
			glUniform1i(glGetUniformLocation(shaderId, uniformName), value);

			// reset the texture
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			std::cout << "Failed to load texture\n";
		}

		// make sure to free up the data
		stbi_image_free(data);
	}

	bool PrimitiveModel::LoadVertices(const char* vertFile)
	{
		std::ifstream verticesFile(vertFile);

		if (!verticesFile.is_open())
		{
			return false;
		}

		float num = 0;
		while (verticesFile >> num)
		{
			this->vertices.push_back(num);
		}

		verticesFile.close();
		return true;
	}

	/*
		bool Renderer::LoadIndices(const char* indices)
		{
			std::ifstream indicesFile(indices);

			if (!indicesFile.is_open())
			{
				return false;
			}

			float num = 0;
			while (indicesFile >> num)
			{
				indicesVector.push_back(num);
			}

			indicesFile.close();
			return true;
		}
	*/

	ModelTexture PrimitiveModel::CreateTexture(std::string name, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter,
								 unsigned int magFilter, unsigned int internalFormat, unsigned int imageFormat)
	{
		ModelTexture texture = {};
		texture.name = name;
		texture.wrapS = wrapS;
		texture.wrapT = wrapT;
		texture.minFilter = minFilter;
		texture.magFilter = magFilter;
		texture.internalFormat = internalFormat;
		texture.imageFormat = imageFormat;
		textures[name] = texture;
		return textures[name];
	}

	void PrimitiveModel::InitTextureNormalCubeData(unsigned int shaderId)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		ModelTexture tex1 = CreateTexture("diffuseMap", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB, GL_RGB);
		LoadTexture(tex1, shaderId, "content/textures/container2.png", "material.diffuse", 0);
		ModelTexture tex2 = CreateTexture("specularMap", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB, GL_RGB);
		LoadTexture(tex2, shaderId, "content/textures/container2_specular.png", "material.specular", 1);

		// NOTE(CK): Already doing this in LoadTexture
		//glUseProgram(shaderId);
		//glUniform1i(glGetUniformLocation(shaderId, "material.diffuse"), 0);
		//glUniform1i(glGetUniformLocation(shaderId, "material.specular"), 1);

		// put into models texture map
		textures[tex1.name] = tex1;
		textures[tex2.name] = tex2;
	}


	void PrimitiveModel::InitTextureCubeData(unsigned int shaderId)
	{
		// void InitRenderTextureData(std::vector<float>& vertices, unsigned int shaderId, unsigned int& VBO, unsigned int& VAO)
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// texture coords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		ModelTexture tex = CreateTexture("tex", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGB, GL_RGB);
		LoadTexture(tex, shaderId, "content/textures/wall.jpg", "texture1", 0);
	}

	void PrimitiveModel::InitBasicCubeData(unsigned int shaderId)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

		glBindVertexArray(VAO);

		// Position Attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// Normal Attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
}