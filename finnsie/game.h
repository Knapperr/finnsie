#ifndef GAME_HG_
#define GAME_HG_

#include "cube_map.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace finnsie {
    
	class Game
	{
    public:
		Game(GLFWwindow& wnd);
		void Render();
		void Update(float dt);
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

		// Terrain 
		// Lights

        WaterObject* testSphere;
        WaterObject* distortWater;
		WaterObject* dirWater;
		
		Cubemap cubemap;

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

		void processCamera(float dt);
	};


	// Easier access for GUI
	extern Game *g_Game;
}

#endif