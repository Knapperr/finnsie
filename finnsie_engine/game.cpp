#include "game.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include "utils.h";

namespace finnsie {

	void Game::Init(GLFWwindow &wnd)
	{
		this->mode = Mode::EDIT;
		this->window = &wnd;
		this->gui = new Gui();
		this->renderer = new Renderer();
		this->camera = new Camera();

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

		// get info from gui
		isGuiHovered = gui->state.active;

		// Update game with data from gui
		if (!g_models.empty())
		{
			g_models[gui->state.modelInfo.index]->SetInfo(gui->state);
		}
	}

	void Game::Render()
	{
		renderer->BeginRender(*camera);
			for (unsigned int i = 0; i < g_models.size(); i++)
			{
				// can send through view normals with the model instead of through the method
				renderer->DrawModel(*g_models[i]);
			}
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
		renderer->Shutdown();
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