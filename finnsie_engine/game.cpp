#include "game.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include "utils.h";

namespace finnsie {
	void Game::Init(GLFWwindow &wnd)
	{
		::finnsie::g_resourceManager = new ResourceManager();

		this->mode = Mode::EDIT;
		this->window = &wnd;
		this->gui = new Gui();
		this->renderer = new Renderer();
		this->camera = new Camera();

		this->projection = glm::mat4(1.0f);
		this->view = glm::mat4(1.0f);

		// Need a shader for models
		this->modelShader = ::finnsie::g_resourceManager->GenerateShader(001, "vert_model.glsl", "frag_model.glsl", NULL);

		// uniforms
		this->objProjLoc = glGetUniformLocation(modelShader.id, "projection");
		this->objViewLoc = glGetUniformLocation(modelShader.id, "view");
		this->objModelLoc = glGetUniformLocation(modelShader.id, "model");
		
		gui->Init(*this->window, camera->MovementSpeed);

		// Load models based off of text file
		//LoadModels("PATHHERE");

	}

	// TODO(CK): Wont need this if i just put the gui object in this class.... 
	// TODO(CK): Not sure if the gui should be in this class or not
	void Game::SetGuiState(gui_state &state)
	{
		//guiState = &state;
	}

	void Game::Update(float dt)
	{
		gui->Update();

		// Do this first
		if (leftMousePressed)
		{
			processCamera(dt);
		}

		isGuiHovered = gui->state.active;

		// Update game with data from gui
		if (!g_models.empty())
		{
			g_models[gui->state.modelInfo.index]->scale = gui->state.modelInfo.scale;
		}
	}

	void Game::Render()
	{
		projection = glm::perspective(glm::radians(camera->Zoom),
									 (float)1080 / (float)720,
									 1.0f, 1000.0f); // NOTE(CK): near and far clipping distance
		view = camera->GetViewMatrix();

		// render the model last
		glUseProgram(modelShader.id);
		// need to set the view and projection as well
		glUniformMatrix4fv(objProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(objViewLoc, 1, GL_FALSE, glm::value_ptr(view));

		for (unsigned int i = 0; i < g_models.size(); i++)
		{
			renderer->DrawModel(*g_models[i], modelShader.id, objModelLoc);
		}


		// render gui
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
		// This interacts with the gui well because if the mouse button is held down
		// the gui wont register to the mouse
		if (!isGuiHovered && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
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
		delete renderer;
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


}