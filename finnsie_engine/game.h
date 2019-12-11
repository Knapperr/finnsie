#ifndef GAME_HG_
#define GAME_HG_

#include "global.h"

#include "game_object.h"
#include "sprite_renderer.h"

#include <vector>

namespace finnsie {

	class Game 
	{
	public:
		unsigned int Width;
		unsigned int Height;
		
		std::vector<GameObject> gameObjects;

		Game(unsigned int width, unsigned int height, GLFWwindow* window);
		~Game();

		void Init();
		void Render();
		void Update();

	private:
		GLFWwindow* window;
		SpriteRenderer* renderer;
	};
	
}

#endif