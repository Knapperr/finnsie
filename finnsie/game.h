#ifndef GAME_HG_
#define GAME_HG_

#include "cube_map.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace finnsie {
    
	struct Input
	{
		bool up;
		bool down;
		bool left;
		bool right;
		bool forward;
		bool back;
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

		GLFWwindow* window;
		Gui *gui;
		
		Renderer *renderer;
		Camera *camera;

		Terrain *terrain;
		// Lights

        WaterObject* testSphere;
        WaterObject* distortWater;
		WaterObject* dirWater;
		
		Cubemap cubemap;

	private:



        // Shader[]
		// TODO(CK):
		// Put into resource manager
        Shader modelShader;
        Shader normalShader;
        Shader waterShader;
        Shader waterDirShader;
        Shader binnShader; // used for terrain for now
		Shader grassShader;
		Shader cubemapShader;

		void initObjects();
		void initShaders();
		void processCamera(float dt);
	};


	// Easier access for GUI
	extern Game *g_Game;
}

#endif