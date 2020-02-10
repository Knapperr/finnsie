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

	void Renderer::DrawTextureCube(unsigned int shaderId, Model textureCube, glm::vec3 cubePositions[],
		int projLoc, int viewLoc, int modelLoc, glm::mat4 projection, glm::mat4 view)
	{
		glUseProgram(shaderId);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glBindVertexArray(textureCube.VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	void Renderer::DrawLamp(unsigned int shaderId, Model lightCube, unsigned int lightModelLoc,
							unsigned int lightProjLoc, unsigned int lightViewLoc, glm::mat4 projection,
							glm::mat4 view, glm::vec3 lampPos)
	{
		glUseProgram(shaderId);
		glUniformMatrix4fv(lightProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lampPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(lightCube.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void Renderer::DrawTextureNormalCube(unsigned int shaderId, Model textureCube,
		int projLoc, int viewLoc, int modelLoc, glm::mat4 projection, glm::mat4 view) 
	{

	}
}