#include "game.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

namespace finnsie {
    
	// Game inits resource manager
	ResourceManager* g_resourceManager;
    // TODO(CK): Figure something out for lights in game
    // keep in their own vector for awhile?
    glm::vec3 g_lamp;
    
	Game::Game(GLFWwindow& wnd)
	{
		LOG("Game Init");
        
		::finnsie::g_resourceManager = new ResourceManager();
        g_lamp = glm::vec3(1.0f);
		this->mode = Mode::EDIT;
		this->window = &wnd;
		this->gui = new Gui();
		this->renderer = new Renderer();
		this->camera = new Camera();
		// TODO(CK): Use game camera
		this->gameCamera = new Camera();
		this->leftMousePressed = false;
		
		gui->Init(*this->window, camera->MovementSpeed);
		
		
		distortWater = new WaterObject("water",
                                       glm::vec3(-100.0f, -30.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f),
                                       40.0f,
                                       "content/objects/quad/basic_quad.obj");
		LoadDistortedWater(distortWater->model);
        
		dirWater = new WaterObject("water",
                                   glm::vec3(-100.0f, -30.0f, 80.0f),
                                   glm::vec3(0.0f, glm::radians(180.0f), 0.0f),
                                   40.0f,
                                   "content/objects/quad/basic_quad.obj");
		LoadDirectionalWater(dirWater->model);
        
        testSphere = new GameObject("sphere",
                                    glm::vec3(-100.0f, 0.0f, 80.0),
                                    glm::vec3(0.0f, 0.0f, 0.0f),
                                    10.0f,
                                    "content/objects/sphere/sphere.obj");
        std::string diffusePath = "content/textures/wall.jpg";
        std::string diffuseDir = diffusePath.substr(0, diffusePath.find_last_of('/'));
        Texture text = {};
        text.id = LoadTextureFile("wall.jpg", diffuseDir, false);
        text.type = "texture_diffuse";
        text.path = diffusePath;
        testSphere->model->meshes[0].textures.push_back(text);
        
	}
    
	void Game::Update(float dt)
	{
        // TODO(CK): Pass water for now... find a way to update
        // the game data directly... get lighting info directly too
        // TODO(CK): Gui needs some sort of gamestate
		gui->Update(*distortWater, *dirWater);
        
		if (leftMousePressed)
		{
			processCamera(dt);
		}
	}
    
	void Game::Render()
	{
		// TODO(CK): Use game camera (can switch to gameCamera here)
		renderer->BeginRender(*camera); 
        
        for (unsigned int i = 0; i < g_objects.size(); i++)
        {
            if (g_objects[i]->model != NULL)
            {
                renderer->DrawModel(*g_objects[i]);
            }
            
        }
		renderer->DrawWater(distortWater);
		renderer->DrawDirWater(dirWater);
        renderer->DrawSphere(*testSphere);
		renderer->EndRender();
		gui->Render();
	}
    
	void Game::ProcessInput(int key, int action, int scancode, int mods, float dt)
	{
		/*
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		{
			// Change to (!cameraMode) - camera mode else guimode
			if (mode == Mode::CAMERA)
			{
				mode = Mode::GUI;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				std::cout << "normal\n";

			}
			else
			{
				mode = Mode::CAMERA;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				std::cout << "disabled\n";
			}
		}
		*/
	}
    
	void Game::ProcessMouseButtons(int button, int action, int mods)
	{
		// NOTE(CK): (hack?)
        // TODO(CK): Need to not allow changing camera speed while gui is active
		// This interacts with the gui well because if the mouse button is held down
		// the gui wont register to the mouse
		if (!gui->Active() && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			leftMousePressed = true;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			leftMousePressed = false;
		}
	}
    
	void Game::Shutdown()
	{
		renderer->Shutdown();
		delete renderer;
        
		delete distortWater;
		delete dirWater;
        delete testSphere;
        
		delete camera;
		delete gameCamera;
		gui->Shutdown();
		delete gui;
	}
	
	void Game::processCamera(float dt)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::FORWARD, dt); }
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::BACKWARD, dt); }
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::LEFT, dt); }
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::RIGHT, dt); }
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::UP, dt); }
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::DOWN, dt); }
	}
}