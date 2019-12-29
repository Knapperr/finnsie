#ifndef _RENDER_HELPERS_HG_
#define _RENDER_HELPERS_HG_

#include "global.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <vector>

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

void InitRenderData(std::vector<float>& vertices, unsigned int& VBO, unsigned int& cubeVAO, unsigned int& lampVAO)
{
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


	// Texture
	//float vertices[] = {
	//	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	//	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	//	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	//	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	//	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	//	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	//	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	//	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	//	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	//	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	//};

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


	// NOTE: THIS WAS 5 BEFORE because its based off of the vertices with texture coords
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	// Texture coord attributes
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

}

#endif
