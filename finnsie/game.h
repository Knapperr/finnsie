#ifndef GAME_HG_
#define GAME_HG_

#include "camera.h"
#include "gui.h"
#include "renderer.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace finnsie {
    
    // I don't think the cubemap needs to be a class
    // you could make multiple cubemaps and cycle through them!
    // better to pass this one around
    struct Cubemap
    {
        // delete these buffers
        unsigned int VAO;
        unsigned int VBO;
        unsigned int textureID;
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