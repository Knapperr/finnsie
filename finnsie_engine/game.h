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

	/*
	
	INSTEAD OF DOING THIS WE CAN JUST CREATE A WATER GameObject
	// that has all this waterInfo in it!
	
	// vector of models gets changed to a vector of GameObjects
	// then we can edit that directly
	
	class WaterModel : public Model
	{
		struct WaterInfo
		{
			float uJump = 0.25f;
			float vJump = 0.25f;
			float tiling = 1.0f;
			float speed = 0.2f;
			float flowStrength = 0.07f;
			float flowOffset = -0.207f;
			float heightScale = 0.1f;
			float heightScaleModulated = 9.0f;
			float gridResolution = 10.0f;
			float tilingModulated = 50.0f;
			bool dualGrid = false;
		};

		WaterModel() : Model()
		{
			WaterInfo* waterInfo = new WaterInfo();
		}

		WaterModel(std::string name, glm::vec3 pos, glm::vec3 orientation, float scale, std::string path) : Model(name, pos, orientation, scale, path)
		{
			WaterInfo* waterInfo = new WaterInfo();
		}
	};
	
	*/

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
		GameObject* distortWater;
		GameObject* dirWater;
		draw_info drawInfo;

		void processCamera(float dt);
	};
}

#endif