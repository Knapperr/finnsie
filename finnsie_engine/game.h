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

	struct game_state
	{
		int selectedModel;
	};

	struct GameObject
	{
		// This design makes more sense the model should be a 
		// component of the game object this way you can attach
		// components onto an object. You can a model then that model
		// can have like a light attached to it or something

		// The draw information should still be on the model
		// cause that information is used to draw the actual model???
		Model* model;
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
		game_state state;
		GLFWwindow* window;
		Camera *camera;
		Camera* gameCamera;
		Renderer *renderer;

	private:
		Gui *gui;
		Model* water;
		Model* dirWater;
		draw_info drawInfo;

		void processCamera(float dt);
	};
}

#endif