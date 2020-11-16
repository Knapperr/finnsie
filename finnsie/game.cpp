#include "game.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <time.h>

namespace finnsie {

    // TODO(CK): Make these part of the game class
    // the game is now global instead
    glm::vec3 g_lamp;

	Game::Game(GLFWwindow& wnd)
	{
		LOG("Game Init");
        
        g_lamp = glm::vec3(1.0f);
        g_lamp.x = 224.0f;
        g_lamp.y = 557.38f;

		this->window = &wnd;
		this->gui = new Gui();
		this->renderer = new Renderer();
        this->camera = new Camera();

		this->leftMousePressed = false;
        this->debugRightMousePressed = false;
		this->debugSphereStopped = false;
		
        ResourceManager::Init();

        modelShader = ResourceManager::GetShader("model");
        disWaterShader = ResourceManager::GetShader("waterDis");
        dirWaterShader = ResourceManager::GetShader("waterDir");
        lightShader = ResourceManager::GetShader("light");

        initObjects();
        pInput = {};
        this->followCamera = new ThirdPersonCamera(player);
        this->followCameraActive = true;

        terrain = new Terrain(0, 0);
        terrain->Generate();
        terrain->GenerateGrass();

        this->cubemap = {};
        SetupCubemap(&this->cubemap);

        this->gui->Init(*this->window);
	}
    
    void Game::GuiUpdate()
    {
        gui->Update();
    }

    // TODO(CK): pass the input from main to here
	void Game::Update(float dt)
	{        
		if (leftMousePressed && !followCameraActive)
		{
			processCamera(dt);
		}
        
        if (followCameraActive)
        {
            processPlayer(dt);
            followCamera->Move();
        }

        light->pos.x = g_lamp.x;
        light->pos.y = g_lamp.y;
        light->pos.z = g_lamp.z;
	}

    void Game::GuiRender()
    {
        gui->Render();
    }
    
	void Game::Render()
	{
        // TODO(CK): SOLUTION FOR NOW CLEAN UP
        if (followCameraActive)
        {
            renderer->BeginRender(*followCamera);
        }
        else
        {
		    renderer->BeginRender(*camera); 
        }

        for (unsigned int i = 0; i < g_objects.size(); i++)
        {
            if (g_objects[i]->model != NULL)
            {
                renderer->DrawModel(*g_objects[i], *modelShader);
            }
        }
        // draw above water so you can see underneath
        renderer->DrawTerrain(terrain);
        renderer->DrawWater(distortWater, *ResourceManager::GetShader("waterDis"));
		//renderer->DrawDirWater(dirWater, waterDirShader);
        renderer->DrawModel(*light, *lightShader);
        
        renderer->DrawSphere(*cube, *ResourceManager::GetShader("binn"));

        renderer->DrawModel(*player, *modelShader);

        DrawCubemap(&this->cubemap, ResourceManager::GetShader("cubemap"), this->camera);
		renderer->EndRender();
	}
    
	void Game::ProcessInput(int key, int action, int scancode, int mods, float dt)
	{
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            Reload(ResourceManager::GetShader("binn"));
            Reload(ResourceManager::GetShader("waterDis"));
        }
        
        // How to use the delta time properly for movement.. is movmenet in a loop?
        // handmade 
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
        { 
            g_lamp.x -= 85 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) 
        { 
            g_lamp.x += 85 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) 
        {
            g_lamp.z += 85 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) 
        { 
            g_lamp.z -= 85 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        {
            g_lamp.y += 85 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        {
            g_lamp.y -= 85 * dt;
        }

	}
    
	void Game::ProcessMouseButtons(int button, int action, int mods)
	{
		// NOTE(CK): (hack?)
		// This interacts with the gui well because if the mouse button is held down
		// the gui wont register to the mouse
		if (!gui->Active() && 
            button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			leftMousePressed = true;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			leftMousePressed = false;
		}
       


        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            debugRightMousePressed = true;
        }
        else
        {
            debugRightMousePressed = false;
        }
	}
    
	void Game::Shutdown()
	{

        ResourceManager::Shutdown();
        DestroyCubemap(&this->cubemap);

        renderer->Shutdown();
		delete renderer;
        
        delete player;
		delete distortWater;
		delete dirWater;
        delete light;

		delete camera;
		gui->Shutdown();
		delete gui;
	}

	void Game::processCamera(float dt)
	{
        // Camera Movement
        // ===========================================================
        bool speedIncreased = false;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            camera->MovementSpeed *= 5;
            speedIncreased = true;
        }

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::FORWARD, dt); }
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::BACKWARD, dt); }
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::LEFT, dt); }
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::RIGHT, dt); }
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::UP, dt); }
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::DOWN, dt); }

        if (speedIncreased)
            camera->MovementSpeed /= 5;
        // ===========================================================
	}

    void Game::processPlayer(float dt)
    {
        player->ResetInput(&pInput);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { pInput.forward = true;}
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { pInput.backward = true;}
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { pInput.left = true;}
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { pInput.right = true;}

        player->Move(pInput, dt);
    }

    void Game::initObjects()
    {
        // Water objects
        distortWater = new WaterObject("water",
                                       glm::vec3(200.0f, 0.5f, 190.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f),
                                       40.0f,
                                       "content/objects/quad/basic_quad.obj");
        LoadDistortedWater(distortWater);

        dirWater = new WaterObject("water",
                                   glm::vec3(100.0f, 5.0f, 280.0f),
                                   glm::vec3(0.0f, glm::radians(180.0f), 0.0f),
                                   40.0f,
                                   "content/objects/quad/basic_quad.obj");
        LoadDirectionalWater(dirWater);

        // Test Sphere for shader learning 
        light = new WaterObject("sphere",
                                glm::vec3(-100.0f, 100.0f, 80.0),
                                glm::vec3(0.0f, 0.0f, 0.0f),
                                10.0f,
                                "content/objects/sphere/sphere.obj");
        LoadDistortedWater(light);

        cube = new GameObject("cube",
                              glm::vec3(100.0f, 10.0f, 10.0),
                              glm::vec3(0.0f, 0.0f, 0.0),
                              1.0f,
                              "content/objects/cube/cube.obj");

        player = new Player("player",
                            glm::vec3(40.0f, 0.1f, 10.0),
                            glm::vec3(0.0f, 0.0f, 0.0),
                            4.0f,
                            "content/objects/Player/Player.obj");
    }
}