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
        
        // GAME NEEDS A WORLD THAT ALL THE OBJECTS ARE IN
        // NEED A WAY TO DRAW EACH THING GETWATER function .. maybe dont want too many of those or know where they are 
        // in the structure

        // cause we need to have data for the world like how should terrain and stuff be kept
        /*
            could pack the shader in with the object.. the object points to a shader from
            the resource manager. dont duplicate the shader use the same address that way 
            its one access point.. a pointer for every object? bad idea?

            The renderer calls game object and a switch checks what the component is and call the 
            correct draw fhcntion .... Why didn't I think of this before. Could make a gettype function and then it switched off of that 
            Renderer doesn't need camera it needs the cameras actually data like the matrix... Just send that over instead so camera type 
            doesn't even matter it doesn't actually care about a camera object just its data 
        */
        ResourceManager::Init();

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
		

        initObjects();
        pInput = {};
        this->followCamera = new ThirdPersonCamera(player);
        this->followCameraActive = true;

        terrain = new Terrain(0, 0);
        terrain->Generate();
        terrain->GenerateGrass();

        mousePicker = MousePicker(terrain);

        this->cubemap = {};
        SetupCubemap(&this->cubemap);

        this->gui->Init(*this->window);
	}
    
    void Game::GuiUpdate()
    {
        gui->Update();
    }

    // TODO(CK): pass the input from main to here
	void Game::Update(float dt, Input* input)
    {
        ProcessInput(input, dt);
        if (followCameraActive)
        {
            processPlayer(dt);
            followCamera->Move();
        }

        mousePicker.Update(mouseX, mouseY, camera->GetViewMatrix(), renderer->projection, camera->Position);
        //std::cout << "Ray x: " << mousePicker.currentRay.x << " y: " << mousePicker.currentRay.y << " z: " << mousePicker.currentRay.z << '\n';
        //std::cout << "Terrain point x: " << mousePicker.currentTerrainPoint.x << " y: " << mousePicker.currentTerrainPoint.y << " z: " << mousePicker.currentTerrainPoint.z << '\n';


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

        // TODO(ck): one camera pointer to two different cameras that the player can use
        if (followCameraActive)
        {
            renderer->BeginRender(followCamera->distanceFromTarget, followCamera->ViewMatrix(), followCamera->position);
        }
        else
        {
            renderer->BeginRender(camera->Zoom, camera->GetViewMatrix(), camera->Position);; 
        }

        // TODO(CK):
        // SORT OBJECTS FIRST
        
        // Dispatch objects
        for (unsigned int i = 0; i < objects.size(); i++)
        {
            if (objects[i]->model != NULL)
            {
                renderer->Draw(objects[i]);
            }
        }
 
        renderer->DrawModel(*player);
        renderer->DrawTerrain(terrain);
        // TODO(CK): Move to renderer
        DrawCubemap(&this->cubemap, ResourceManager::GetShader("cubemap"), this->camera);
		renderer->EndRender();
	}
    

    // These Process Input & Mouse Functions get called on glfw callbacks
    // ============================================================================
    void Game::ProcessInput(Input* input, float dt)
    {
        if (leftMousePressed && !followCameraActive)
        {
            processCamera(input, dt);
        }


        if (debugRightMousePressed && !followCameraActive)
        {
            mousePicker.currentTerrainPoint.y = 0;
            distortWater->pos = mousePicker.currentTerrainPoint;
        }
        return;
    }
        
    void Game::ProcessMouseButtons(Input* input)
    {
        // NOTE(CK): (hack?)
        // This interacts with the gui well because if the mouse button is held down
        // the gui wont register to the mouse
        if (!gui->Active() && input->mouseButtons[0].endedDown)
        {
            // TODO(ck): Input mode cant be in the game layer
            // Create a windows setting struct for options like this
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            // TODO(CK): dont need this leftMousePressed 
            // change to canProcessCamera or something
            leftMousePressed = true;
        }
        else
        {
            // TODO(ck): Input mode cant be in the game layer
            // Create a windows setting struct for options like this
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            leftMousePressed = false;
        }

        if (input->mouseButtons[1].endedDown)
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

        // clean up objects
        for (unsigned int i = 0; i < objects.size(); ++i)
        {
            delete objects[i];
        }
        objects.clear();
        
        delete player;
		delete camera;
		gui->Shutdown();
		delete gui;
	}

    void Game::ProcessMousePosition(Input* input)
    {
        if (leftMousePressed && !followCameraActive)
        {
            camera->ProcessMouseMovement(input->mouseXOffset, input->mouseYOffset);
        }
        else
        {
            followCamera->ProcessMouse(input->mouseXOffset, input->mouseYOffset);
        }
        // TODO(ck): Hack for now..
        mouseX = input->mouseX;
        mouseY = input->mouseY;
    }

    void Game::ProcessMouseScroll(Input* input)
    {
        if (!gui->Active() && !followCameraActive)
        {
            camera->ProcessMouseScroll(input->yScrollOffset);
        }
        else if (followCameraActive)
        {
            followCamera->CalculateZoom(input->yScrollOffset);
        }
    }

	void Game::processCamera(Input* input, float dt)
	{
        bool speedIncreased = false;
        if (input->controller.actionUp.endedDown)
        {
            camera->MovementSpeed *= 5;
            speedIncreased = true;
        }

		if (input->controller.moveForward.endedDown)    { camera->ProcessKeyboard(Camera_Movement::FORWARD, dt); }
		if (input->controller.moveBackward.endedDown)   { camera->ProcessKeyboard(Camera_Movement::BACKWARD, dt); }
		if (input->controller.moveLeft.endedDown)       { camera->ProcessKeyboard(Camera_Movement::LEFT, dt); }
		if (input->controller.moveRight.endedDown)      { camera->ProcessKeyboard(Camera_Movement::RIGHT, dt); }
		if (input->controller.moveUp.endedDown)         { camera->ProcessKeyboard(Camera_Movement::UP, dt); }
		if (input->controller.moveDown.endedDown)       { camera->ProcessKeyboard(Camera_Movement::DOWN, dt); }

        if (speedIncreased)
            camera->MovementSpeed /= 5;
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
                                       glm::vec3(200.0f, 0.0f, 190.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f),
                                       40.0f,
                                       "content/objects/circle/circle.obj");
        LoadDistortedWater(distortWater);

        dirWater = new WaterObject("water",
                                   glm::vec3(100.0f, 0.1f, 280.0f),
                                   glm::vec3(0.0f, glm::radians(180.0f), 0.0f),
                                   40.0f,
                                   "content/objects/quad/basic_quad.obj");
        LoadDirectionalWater(dirWater);

        light = new WaterObject("sphere",
                                glm::vec3(-100.0f, 100.0f, 80.0),
                                glm::vec3(0.0f, 0.0f, 0.0f),
                                10.0f,
                                "content/objects/sphere/sphere.obj");
        LoadDistortedWater(light);

        GameObject* cube = new GameObject("cube",

                              glm::vec3(100.0f, 10.0f, 10.0),
                              glm::vec3(0.0f, 0.0f, 0.0),
                              1.0f,
                              "content/objects/cube/cube.obj");

        player = new Player("player",
                            glm::vec3(40.0f, 0.1f, 10.0),
                            glm::vec3(0.0f, 0.0f, 0.0),
                            4.0f,
                            "content/objects/Player/Player.obj");


        objects.push_back(distortWater);
        objects.push_back(dirWater);
        objects.push_back(cube);
        objects.push_back(light);
    }
}