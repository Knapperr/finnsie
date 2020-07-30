#ifndef GAME_HG_
#define GAME_HG_


#include "camera.h"
#include "gui.h"
#include "renderer.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

namespace finnsie {
    
    class Terrain
    {
    public:
        float x;
        float z;

        unsigned int vao;
        unsigned int indicesLength;

        Terrain();
        Terrain(int gridX, int gridZ);
        void Generate();
        void Render(Shader* modelShader, Camera* cam);

        /*
        remember to clean up
        delete arrays and buffers from terrain
        glDeleteVertexArrays(1, &this->cubeVAO);
        glDeleteVertexArrays(1, &this->lampVAO);
        glDeleteBuffers(1, &this->VBO);
        
        */
    };


	enum class Mode {
		EDIT,
		RUN
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
		Mode mode;
		GLFWwindow* window;
		Camera *camera;
		Renderer *renderer;
		Gui *gui;
        
        private:
        GameObject* testSphere;
		//GameObject* book;
        WaterObject* distortWater;
		WaterObject* dirWater;
        
        Shader modelShader;
        Shader normalShader;
        Shader waterShader;
        Shader waterDirShader;
        Shader binnShader;

		Terrain terrain;
                
		void processCamera(float dt);

	};
}

#endif