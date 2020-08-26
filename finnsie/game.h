#ifndef GAME_HG_
#define GAME_HG_

#include "camera.h"
#include "gui.h"
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
		Camera *camera;
		Renderer *renderer;
		Gui *gui;
        
    private:
        WaterObject* testSphere;
        WaterObject* distortWater;
		WaterObject* dirWater;
		
		Cubemap cubemap;

        // Shader[]
        Shader modelShader;
        Shader normalShader;
        Shader waterShader;
        Shader waterDirShader;
        Shader binnShader;
		Shader cubemapShader;

		void processCamera(float dt);
	};
}

#endif