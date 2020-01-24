#include "renderer.h"

#include "global.h"

#include <fstream>

namespace finnsie {

	Renderer::Renderer()
	{
		this->color = glm::vec3(0.1f, 0.5f, 0.31f);
		this->colorChange = 0.01f;
		this->lampPos = glm::vec3(1.2f, 1.0f, 2.0f);
	}

	Renderer::~Renderer()
	{
		glDeleteVertexArrays(1, &this->cubeVAO);
		glDeleteVertexArrays(1, &this->lampVAO);
		glDeleteBuffers(1, &this->VBO);
	}

	void Renderer::DrawCube(unsigned int shaderId)
	{
		// NOTE: CANT DO THIS HERE  Activate shader first
		// glUseProgram(shaderId);

		this->color.x += this->colorChange;

		if (this->color.x > 1.0f)
		{
			this->colorChange = -0.01f;
		}
		else if (this->color.x < 0.0f)
		{
			this->colorChange = 0.01f;
		}

		//glUniform3f(glGetUniformLocation(shaderId, "objectColor"), 1.0f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(shaderId, "objectColor"), this->color.x, this->color.y, this->color.z);
		glUniform3f(glGetUniformLocation(shaderId, "lightColor"), 1.0f, 1.0f, 1.0f);

		glm::mat4 model = glm::mat4(1.0f);
		int modelLoc = glGetUniformLocation(shaderId, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(this->cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void Renderer::DrawLamp(unsigned int shaderId)
	{
		// NOTE: Activate shader first
		// CANT DO THIS HERE
		//glUseProgram(shaderId);
		
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, this->lampPos);
		model = glm::scale(model, glm::vec3(0.2f));
		int modelLoc = glGetUniformLocation(shaderId, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(this->lampVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	bool Renderer::LoadVertices(const char* vertices)
	{
		std::ifstream verticesFile(vertices);
		
		if (!verticesFile.is_open())
		{
			return false;
		}

		float num = 0;
		while (verticesFile >> num)
		{
			verticesVector.push_back(num);
		}
	
		verticesFile.close();
		return true;
	}

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

	
	void Renderer::InitRenderData()
	{
		glGenVertexArrays(1, &this->cubeVAO);
		glGenBuffers(1, &this->VBO);
		
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, verticesVector.size() * sizeof(verticesVector), &verticesVector[0], GL_STATIC_DRAW);

		glBindVertexArray(this->cubeVAO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);


		// NOTE: We dont need to define the vertices
		//		 again because the VBO is the same and the lamp is a cube

		//second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
		glGenVertexArrays(1, &this->lampVAO);
		glBindVertexArray(this->lampVAO);
		//// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);


		// NOTE: THIS WAS 5 BEFORE because its based off of the vertices with texture coords
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		// Texture coord attributes
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);

	}
	

	void Renderer::InitRenderLampData()
	{
		// NOTE: THIS MIGHT NOT WORK 



	}
}