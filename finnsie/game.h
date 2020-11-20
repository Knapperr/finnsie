#ifndef GAME_HG_
#define GAME_HG_

#include "cube_map.h"
#include "resource_manager.h"
#include "player.h"
#include "third_person_camera.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace finnsie {
    

	struct ButtonState
	{
		int halfTransitionCount;
		bool endedDown;
	};

	struct ControllerInput
	{
		bool isConnected;
		float stickAverageX;
		float stickAverageY;
		
		union
		{
			bool buttons[14];
			struct
			{
				ButtonState moveForward;
				ButtonState moveBackward;
				ButtonState moveUp;
				ButtonState moveDown;
				ButtonState moveLeft;
				ButtonState moveRight;
				ButtonState actionUp;
				ButtonState actionDown;
				ButtonState actionLeft;
				ButtonState actionRight;
				ButtonState leftShoulder;
				ButtonState rightShoulder;
				ButtonState back;
				ButtonState start;
			};
		};
	};

	struct Input
	{
		ButtonState mouseButtons[5];
		int mouseX;
		int mouseY;
		int mouseZ;
		float xScrollOffset;
		float yScrollOffset;

		// NOTE(ck): one controlelr for now
		ControllerInput controller;
	};

	class Game
	{
    public:
		bool debugRightMousePressed;
		bool debugSphereStopped;
		bool leftMousePressed;
		bool followCameraActive;

		GLFWwindow* window;
		Gui* gui;

		Renderer* renderer;
		Camera* camera;
		ThirdPersonCamera* followCamera;

		Terrain* terrain;
		// Lights

		Player* player;
		PlayerInput pInput;

		WaterObject* light;
		WaterObject* distortWater;
		WaterObject* dirWater;

		GameObject* cube;

		Cubemap cubemap;

		// Core shaders
		Shader* modelShader;
		Shader* disWaterShader;
		Shader* dirWaterShader;
		Shader* lightShader;

		Game(GLFWwindow& wnd);
		void Update(float dt, Input* input);
		void Render();

		// TODO(CK): Pull the gui out of the game
		// we have logic though in the gui that relies on 
		// the game 
		void GuiUpdate();
		void GuiRender();
		
		void ProcessInput(Input* input, float dt);
		void ProcessMousePosition(Input* input);
		void ProcessMouseButtons(Input* input);
		void ProcessMouseScroll(Input* input);
		void Shutdown();
       

	private:
		void initObjects();
		void processCamera(Input* input, float dt);
		void processPlayer(float dt);
	};


	// Easier access for GUI
	extern Game *g_Game;
}

#endif