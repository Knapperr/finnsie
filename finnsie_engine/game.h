#ifndef GAME_HG_
#define GAME_HG_


#include "camera.h"
#include "gui.h"
#include "renderer.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

namespace finnsie {

	enum class Mode {
		EDIT,
		RUN
	};

	class Game
	{
	public:
		Game(GLFWwindow& wnd);
		void Render();
		void Update(float dt);
		void ProcessInput(int key, int action, int scancode, int mods, float dt);
		void ProcessMouseButtons(int button, int action, int mods);
		void Shutdown();

		bool leftMousePressed;
		Mode mode;
		GLFWwindow* window;
		Camera *camera;
		Renderer *renderer;

	private:
		Gui *gui;
		Model water;
		draw_info drawInfo;

		void processCamera(float dt);
	};
}

#endif