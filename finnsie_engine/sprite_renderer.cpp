#include "sprite_renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace finnsie {

	SpriteRenderer::SpriteRenderer(Shader& shader)
		: shader(shader)
	{
		initRenderData();
	}

	SpriteRenderer::~SpriteRenderer()
	{
		glDeleteVertexArrays(1, &this->quadVAO);
	}

	void SpriteRenderer::DrawGameObject(Texture sprite, glm::vec2 pos, glm::vec2 size, glm::vec3 color, float rotation)
	{
		glUseProgram(this->shader.id);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(pos, 0.0f));

		model = glm::translate(model, glm::vec3(0.5f * size.x, 0.0f * size.y, 0.0f));
		model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

		model = glm::scale(model, glm::vec3(size, 1.0f));

		int modelLoc = glGetUniformLocation(shader.id, "model");
		int spriteColorLoc = glGetUniformLocation(shader.id, "spriteColor");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(spriteColorLoc, color.x, color.y, color.z);

		//glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glUniform3f(glGetUniformLocation(shader->id, "spriteColor"), gameObject->color.x, gameObject->color.y, gameObject->color.z);

		// Bind(&texture);
		//Bind(&gameObject->sprite);
		glBindTexture(GL_TEXTURE_2D, sprite.id);


		// After init the render data
		glBindVertexArray(this->quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void SpriteRenderer::initRenderData()
	{
		unsigned int VBO;
		float vertices[] =
		{
			// Pos      // Tex
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
		};

		glGenVertexArrays(1, &this->quadVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindVertexArray(this->quadVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}


}