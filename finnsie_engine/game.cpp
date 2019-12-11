#include "game.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

namespace finnsie {


	// Global ResourceManager 
	// have to initialize here
	ResourceManager* g_resourceManager;

	Game::Game(unsigned int width, unsigned int height, GLFWwindow* window) 
		:	Width(width)	, 
			Height(height)	, 
			window(window)
	{
		g_resourceManager = new ResourceManager();
	}
	
	Game::~Game() 
	{
		// clean up
		g_resourceManager->ShutDown();
		delete ::finnsie::g_resourceManager;
		delete renderer;
	}

	void Game::Init()
	{
		// Create a shader
		g_resourceManager->GenerateShader(001, "vert_sprite.glsl", "frag_sprite.glsl", NULL);

		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Width), 
										static_cast<float>(Height), 0.0f, -1.0f, 1.0f);

		// NOTE: Need to call use program before we can access the shader
		glUseProgram(g_resourceManager->GetShader(001).id);

		int imageLoc = glGetUniformLocation(g_resourceManager->GetShader(001).id, "image");
		int projLoc = glGetUniformLocation(g_resourceManager->GetShader(001).id, "projection");
		glUniform1i(imageLoc, 0);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// NOTE: call in two lines rather than 4 EXAMPLE
		//glUniform1i(glGetUniformLocation(shader.id, "image"), 0);
		//glUniformMatrix4fv(glGetUniformLocation(shader.id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


		// Initialize our renderer here
		renderer = new SpriteRenderer(g_resourceManager->GetShader(001));


		//GameObject(std::string, glm::vec2 pos, glm::vec2 size, glm::vec2 velocity,
		//	glm::vec3 color, bool solid, float rotation, Texture sprite);

		g_resourceManager->GenerateTexture(001, "content/sprites/enemy.png", true);
		GameObject* enemy = new GameObject("Enemy", glm::vec2((Width - 64) / 2, (Height - 64) / 2), glm::vec2(64, 64),
						 glm::vec2(0,0), glm::vec3(1.0f, 1.0f, 1.0f), true, 0.0f, g_resourceManager->GetTexture(001));

		gameObjects.push_back(enemy);


	}

	void Game::Render()
	{
		//for (std::vector<GameObject*>::const_iterator *obj = &gameObjects.begin(); obj != &gameObjects.end(); ++obj)
		//{
		//	obj->Draw();
		//}

		for (auto& itObj : gameObjects)
		{
			itObj->Draw(*renderer);
		}
	}

	void Game::Update()
	{

	}

}