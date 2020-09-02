#include "game.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <time.h>

namespace finnsie {

    // GLOBALS
    // ==================================
    // Game inits resource manager
    ResourceManager* g_resourceManager;

    // TODO(CK): Make these part of the game class
    // the game is now global instead
    glm::vec3 g_lamp;

	Game::Game(GLFWwindow& wnd)
	{
		LOG("Game Init");
        
		::finnsie::g_resourceManager = new ResourceManager();
        g_lamp = glm::vec3(1.0f);
        g_lamp.x = 224.0f;
        g_lamp.y = 55.38f;

		this->window = &wnd;
		this->gui = new Gui();
		this->renderer = new Renderer();
        this->camera = new Camera();

		this->leftMousePressed = false;
        this->debugRightMousePressed = false;
		this->debugSphereStopped = false;
		
        initObjects();
        initShaders();

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
		if (leftMousePressed)
		{
			processCamera(dt);
		}

        testSphere->pos.x = g_lamp.x;
        testSphere->pos.y = g_lamp.y;
        testSphere->pos.z = g_lamp.z;
	}

    void Game::GuiRender()
    {
        gui->Render();
    }
    
	void Game::Render()
	{
		renderer->BeginRender(*camera); 

        for (unsigned int i = 0; i < g_objects.size(); i++)
        {
            if (g_objects[i]->model != NULL)
            {
                renderer->DrawModel(*g_objects[i], modelShader);
            }
        }
        // draw above water so you can see underneath
        renderer->DrawTerrain(terrain, &this->binnShader, &this->grassShader);
        //this->terrain.Render(&this->binnShader, this->camera);
		
        renderer->DrawWater(distortWater, waterShader);
		//renderer->DrawDirWater(dirWater, waterDirShader);
        renderer->DrawWater(testSphere, waterShader);
        
        DrawCubemap(&this->cubemap, &this->cubemapShader, this->camera);

		renderer->EndRender();
	}
    
	void Game::ProcessInput(int key, int action, int scancode, int mods, float dt)
	{
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            Reload(&binnShader);
            Reload(&waterShader);
        }
        
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
        { 
            testSphere->pos.x -= 28 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) 
        { 
            testSphere->pos.x += 28 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) 
        { 
            testSphere->pos.z += 28 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) 
        { 
            testSphere->pos.z -= 28 * dt;
        }
	}
    
	void Game::ProcessMouseButtons(int button, int action, int mods)
	{
		// NOTE(CK): (hack?)
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
        delete ::finnsie::g_resourceManager;

        DestroyCubemap(&this->cubemap);

        renderer->Shutdown();
		delete renderer;
        
		delete distortWater;
		delete dirWater;
        delete testSphere;

		delete camera;
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

    void Game::initObjects()
    {
        // Water objects
        distortWater = new WaterObject("water",
                                       glm::vec3(200.0f, 0.5f, 190.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f),
                                       40.0f,
                                       "content/objects/quad/basic_quad.obj");
        distortWater->tiling = 5.0f;
        distortWater->speed = 0.3f;
        distortWater->flowStrength = 0.05f;
        distortWater->flowOffset = -0.23f;
        distortWater->heightScale = 0.1f;
        distortWater->heightScaleModulated = 8.0f;
        LoadDistortedWater(distortWater->model);

        dirWater = new WaterObject("water",
                                   glm::vec3(100.0f, 5.0f, 280.0f),
                                   glm::vec3(0.0f, glm::radians(180.0f), 0.0f),
                                   40.0f,
                                   "content/objects/quad/basic_quad.obj");
        dirWater->tiling = 10.0f;
        dirWater->speed = 0.2f;
        dirWater->flowStrength = 0.07f;
        dirWater->heightScale = 0.92f;
        dirWater->heightScaleModulated = 9.0f;
        dirWater->gridResolution = 30.0f;
        dirWater->tilingModulated = 50.0f;
        dirWater->dualGrid = true;
        LoadDirectionalWater(dirWater->model);

        // Test Sphere for shader learning 
        testSphere = new WaterObject("sphere",
                                     glm::vec3(-100.0f, 100.0f, 80.0),
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     10.0f,
                                     "content/objects/sphere/sphere.obj");
        testSphere->tiling = 5.0f;
        testSphere->speed = 0.3f;
        testSphere->flowStrength = 0.05f;
        testSphere->flowOffset = -0.23f;
        testSphere->heightScale = 0.1f;
        testSphere->heightScaleModulated = 8.0f;
        LoadDistortedWater(testSphere->model);
    }

    void Game::initShaders()
    {
        // Shaders 
        this->modelShader =
            ::finnsie::g_resourceManager->GenerateShader(001,
                                                         "shaders/model_vert.glsl",
                                                         "shaders/model_frag.glsl",
                                                         NULL);
        this->normalShader =
            ::finnsie::g_resourceManager->GenerateShader(002,
                                                         "shaders/onlynormals_model_vert.glsl",
                                                         "shaders/onlynormals_model_frag.glsl",
                                                         "shaders/onlynormals_model_geo.glsl");
        this->waterShader =
            ::finnsie::g_resourceManager->GenerateShader(003,
                                                         "shaders/waterdistortion_vert.glsl",
                                                         "shaders/waterdistortion_frag.glsl",
                                                         NULL);
        this->waterDirShader =
            ::finnsie::g_resourceManager->GenerateShader(004,
                                                         "shaders/waterdirection_vert.glsl",
                                                         "shaders/waterdirection_frag.glsl",
                                                         NULL);
        this->binnShader =
            ::finnsie::g_resourceManager->GenerateShader(005,
                                                         "shaders/blinnphong_vert.glsl",
                                                         "shaders/blinnphong_frag.glsl",
                                                         NULL);
        this->cubemapShader =
            ::finnsie::g_resourceManager->GenerateShader(006,
                                                         "shaders/cubemap_vert.glsl",
                                                         "shaders/cubemap_frag.glsl",
                                                         NULL);

        this->grassShader =
            ::finnsie::g_resourceManager->GenerateShader(007,
                                                         "shaders/grass_vert.glsl",
                                                         "shaders/grass_frag.glsl",
                                                         NULL);
    }
}