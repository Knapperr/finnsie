#ifndef _RENDER_HELPERS_HG_
#define _RENDER_HELPERS_HG_

#include "global.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <vector>
#include <stb_image/stb_image.h>

bool LoadVertices(std::vector<float>& vertices, const char* vertFile)
{
	std::ifstream verticesFile(vertFile);

	if (!verticesFile.is_open())
	{
		return false;
	}

	float num = 0;
	while (verticesFile >> num)
	{
		vertices.push_back(num);
	}

	verticesFile.close();
	return true;
}

bool LoadIndices(std::vector<float>& indices, const char* indFile)
{
	std::ifstream indicesFile(indFile);

	if (!indicesFile.is_open())
	{
		return false;
	}

	float num = 0;
	while (indicesFile >> num)
	{
		indices.push_back(num);
	}

	indicesFile.close();
	return true;
}


// Basic Lighting - https://learnopengl.com/Lighting/Basic-Lighting
// ----------------------------------------------------------------------

void DrawBasicLightCube(unsigned int shaderId, unsigned int cubeVAO, glm::vec3& lightPos, glm::vec3& cameraPos)
{
	glUniform3f(glGetUniformLocation(shaderId, "objectColor"), 1.0f, 0.5f, 0.31f);
	glUniform3f(glGetUniformLocation(shaderId, "lightColor"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(shaderId, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(shaderId, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 model = glm::mat4(1.0f);
	int modelLoc = glGetUniformLocation(shaderId, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// render
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void DrawBasicLightLamp(unsigned int shaderId, unsigned int lampVAO, glm::vec3 lightPos)
{
	int modelLoc = glGetUniformLocation(shaderId, "model");
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lampVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void InitBasicLightingData(std::vector<float>& vertices, unsigned int& VBO, unsigned int& cubeVAO, unsigned int& lightVAO)
{
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Normal Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Configure the light's VAO (VBO is the same because the vertices are the same (both are cubes) ) 
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// update the lamps position attribute's stride to refelct the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// READ FROM FILE
	//float vertices[] = {
	//-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	// 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	// 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	// 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	//-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	// 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	// 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	// 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	//-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	//-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	//-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	// 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	// 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	// 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	// 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	// 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	// 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	//-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	// 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	// 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	// 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	//-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	//-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	//-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	// 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	// 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	// 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	//-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	//-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	//};
}



// Basic Color - https://learnopengl.com/Lighting/Colors
// -----------------------------------------------------

void DrawCube(unsigned int shaderId, unsigned int cubeVAO, glm::vec3& color, float& colorChange)
{
	// NOTE: CANT DO THIS HERE  Activate shader first
	// glUseProgram(shaderId);

	color.x += colorChange;

	if (color.x > 1.0f)
	{
		colorChange = -0.01f;
	}
	else if (color.x < 0.0f)
	{
		colorChange = 0.01f;
	}

	//glUniform3f(glGetUniformLocation(shaderId, "objectColor"), 1.0f, 0.5f, 0.31f);
	glUniform3f(glGetUniformLocation(shaderId, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(shaderId, "lightColor"), 1.0f, 1.0f, 1.0f);

	glm::mat4 model = glm::mat4(1.0f);
	int modelLoc = glGetUniformLocation(shaderId, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void DrawLamp(unsigned int shaderId, unsigned int lampVAO, glm::vec3& lampPos)
{
	// NOTE: Activate shader first
	// CANT DO THIS HERE
	//glUseProgram(shaderId);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, lampPos);
	model = glm::scale(model, glm::vec3(0.2f));
	int modelLoc = glGetUniformLocation(shaderId, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lampVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void InitRenderTextureData(std::vector<float>& vertices, unsigned int shaderId, unsigned int& VBO, unsigned int& VAO)
{
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

	// load and create a texture
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// Set wrapping params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set filtering params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load the image 
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("content/textures/wall.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture\n";
	}
	// make sure to free up the data
	stbi_image_free(data); 

	// Need to tell the shader where the texture belongs
	glUseProgram(shaderId);
	glUniform1i(glGetUniformLocation(shaderId, "texture1"), 0);
}

void InitRenderData(std::vector<float>& vertices, unsigned int& VBO, unsigned int& cubeVAO, unsigned int& lampVAO)
{
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// NOTE: We dont need to define the vertices
	//		 again because the VBO is the same and the lamp is a cube

	//second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);
	//// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// READ FROM FILE
	//float vertices[] = {
	//-0.5f, -0.5f, -0.5f,
	// 0.5f, -0.5f, -0.5f,
	// 0.5f,  0.5f, -0.5f,
	// 0.5f,  0.5f, -0.5f,
	//-0.5f,  0.5f, -0.5f,
	//-0.5f, -0.5f, -0.5f,

	//-0.5f, -0.5f,  0.5f,
	// 0.5f, -0.5f,  0.5f,
	// 0.5f,  0.5f,  0.5f,
	// 0.5f,  0.5f,  0.5f,
	//-0.5f,  0.5f,  0.5f,
	//-0.5f, -0.5f,  0.5f,

	//-0.5f,  0.5f,  0.5f,
	//-0.5f,  0.5f, -0.5f,
	//-0.5f, -0.5f, -0.5f,
	//-0.5f, -0.5f, -0.5f,
	//-0.5f, -0.5f,  0.5f,
	//-0.5f,  0.5f,  0.5f,

	// 0.5f,  0.5f,  0.5f,
	// 0.5f,  0.5f, -0.5f,
	// 0.5f, -0.5f, -0.5f,
	// 0.5f, -0.5f, -0.5f,
	// 0.5f, -0.5f,  0.5f,
	// 0.5f,  0.5f,  0.5f,

	//-0.5f, -0.5f, -0.5f,
	// 0.5f, -0.5f, -0.5f,
	// 0.5f, -0.5f,  0.5f,
	// 0.5f, -0.5f,  0.5f,
	//-0.5f, -0.5f,  0.5f,
	//-0.5f, -0.5f, -0.5f,

	//-0.5f,  0.5f, -0.5f,
	// 0.5f,  0.5f, -0.5f,
	// 0.5f,  0.5f,  0.5f,
	// 0.5f,  0.5f,  0.5f,
	//-0.5f,  0.5f,  0.5f,
	//-0.5f,  0.5f, -0.5f,
	//};
}

#endif
