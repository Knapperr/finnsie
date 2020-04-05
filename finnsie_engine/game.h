#ifndef GAME_HG_
#define GAME_HG_

#include "gl_common.h"
#include "global.h"
#include "renderer.h"
#include "camera.h"
#include "gui.h"

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
		void Init(GLFWwindow& wnd);
		void Render();
		void Update(float dt);
		void SetGuiState(gui_state& state);
		void ProcessInput(int key, int action, int scancode, int mods, float dt);
		void ProcessMouseButtons(int button, int action, int mods);
		void Shutdown();

		Mode mode;
		Shader modelShader;
		bool leftMousePressed;

		GLFWwindow* window;
		Camera *camera;
		Renderer *renderer;

	private:
		glm::mat4 projection;
		glm::mat4 view;
		int objProjLoc;
		int objViewLoc;
		int objModelLoc;
		bool isGuiHovered = true;
		Gui *gui;

		void processCamera(float dt);
	};
}

#endif