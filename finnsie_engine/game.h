#ifndef GAME_HG_
#define GAME_HG_

#include "gl_common.h"
#include "global.h"
#include "renderer.h"
#include "camera.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

namespace finnsie {

	enum class Mode {
		GUI,
		CAMERA
	};

	class Game
	{
	public:
		void Init(GLFWwindow& wnd);
		void Render();
		void Update(float dt);
		void ProcessInput(int key, int action, int scancode, int mods, float dt);
		void Shutdown();

		Mode mode;

		GLFWwindow* window;
		Shader modelShader;
		Camera *camera;
		Renderer *renderer;

		glm::mat4 projection;
		glm::mat4 view;

		int objProjLoc;
		int objViewLoc;
		int objModelLoc;

	private:
		void processCamera(float dt);
	};
}

#endif