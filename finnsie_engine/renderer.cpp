#include "renderer.h"

#include "global.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace finnsie {

	Renderer::Renderer()
	{
		initRenderCubeData();
	}

	Renderer::~Renderer()
	{
		glDeleteVertexArrays(1, &this->cubeVAO);
		//glDeleteBuffers(1, &VBO); this is in our member function for now
	}

	void Renderer::DrawCube(unsigned int shaderId)
	{
		// NOTE: Activate shader first
		glUseProgram(shaderId);
		glUniform3f(glGetUniformLocation(shaderId, "objectColor"), 0.4f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(shaderId, "lightColor"), 1.0f, 1.0f, 1.0f);
		

		glm::mat4 model = glm::mat4(1.0f);
		int modelLoc = glGetUniformLocation(shaderId, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(this->cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void Renderer::initRenderCubeData()
	{
		unsigned int VBO;
		float vertices[] = {
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};

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

		glGenVertexArrays(1, &this->cubeVAO);
		glGenBuffers(1, &VBO);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		glBindVertexArray(this->cubeVAO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);


		// NOTE: THIS WAS 5 BEFORE because its based off of the vertices with texture coords
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		// Texture coord attributes
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);


		//second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
		//unsigned int lightVAO;
		//glGenVertexArrays(1, &lightVAO);
		//glBindVertexArray(lightVAO);

		//// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(0);

	}
}