#ifndef GAME_HG_
#define GAME_HG_

#include "cube_map.h"
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
		float mouseXOffset;
		float mouseYOffset;
		float xScrollOffset;
		float yScrollOffset;

		// NOTE(ck): one controller for now
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
		MousePicker mousePicker;
		// TODO(ck): Hack for the mouse picker going to track mouse pos separate for right now
		// GLFW input stuff sucks. need to find a way to cleanly deal with it? or change frameworks
		float mouseX, mouseY;
		Cubemap cubemap;


		// TODO(ck):
		// Move to world. move player input into player
		// =================================================
		Player* player;
		PlayerInput pInput;
		Terrain* terrain;
		GameObject* currentObject;
		std::vector<GameObject*> objects;

		// NOTE(ck): Objects are pulled oot because 
		// they are used in the GUI
		// TODO(ck): Write an easy way for GUI to access specific
		// objects from vector
		WaterObject* light;
		WaterObject* distortWater;
		WaterObject* dirWater;
		// ==================================================


		Game(GLFWwindow& wnd);
		void Update(float dt, Input* input);
		void Render();

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

	extern Game *g_Game;
}

#endif