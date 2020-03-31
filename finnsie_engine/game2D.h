#ifndef GAME2D_HG_
#define GAME2D_HG_

#include "gl_common.h"
#include "global.h"

#include "game_object.h"
#include "sprite_renderer.h"

#include <vector>

namespace finnsie {

	class Game2D 
	{
	public:
		unsigned int Width;
		unsigned int Height;
		
		std::vector<GameObject> gameObjects;

		Game2D(unsigned int width, unsigned int height, GLFWwindow* window);
		~Game2D();

		void Init();
		void Render();
		void Update();

	private:
		GLFWwindow* window;
		SpriteRenderer* renderer;
	};
	
}

#endif