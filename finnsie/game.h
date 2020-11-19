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
			ButtonState buttons[12];
			struct
			{
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

		// NOTE(ck): one controlelr for now
		ControllerInput controller;
	};

	class Game
	{
    public:
		Game(GLFWwindow& wnd);
		void Update(float dt);
		void Render();

		// TODO(CK): Pull the gui out of the game
		// we have logic though in the gui that relies on 
		// the game 
		void GuiUpdate();
		void GuiRender();
		
		void ProcessInput(int key, int action, int scancode, int mods, float dt);
		void ProcessMouseButtons(int button, int action, int mods);
		void Shutdown();
        
        bool debugRightMousePressed;
        bool debugSphereStopped;
		bool leftMousePressed;
		bool followCameraActive;

		GLFWwindow* window;
		Gui *gui;
		
		Renderer *renderer;
		Camera *camera;
		ThirdPersonCamera* followCamera;

		Terrain *terrain;
		// Lights

		Player* player;
		// TODO(CK): Add controller
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

	private:
		void initObjects();
		void processCamera(float dt);
		void processPlayer(float dt);
	};


	// Easier access for GUI
	extern Game *g_Game;
}

#endif