#include "gl_common.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "camera.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "game.h"
#include "primitive_model.h"
#include "model.h"
#include "renderer.h"
#include "utils.h"
#include "gui.h"

using namespace finnsie;

void processInput(GLFWwindow* window, int key, int action, int scancode, int mods);

const unsigned int SCREEN_WIDTH = 1080;
const unsigned int SCREEN_HEIGHT = 720;

// NOTE(CK): global resource manager
ResourceManager* g_resourceManager = NULL;

Gui* gui = new Gui();
Game* game = new Game();

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

float g_playerXPos = 0;
float g_velocity = 300.0f;

struct DeltaTime
{
	float time;
	float lastFrame;
};
global_variable DeltaTime dt;

static void error_callback(int error, const char* description);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void gamepad_callback(int jid, int event);
// not using anymore
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main(int argc, char** argv)
{
	//---------------------------------------------------------------------
	// glfw init and config
	// --------------------
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// NOTE: Only works on major 3 and minor 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Finnsie", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, processInput);
	glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);
	//glfwSetJoystickCallback(gamepad_callback);


	// tell GLFW to capture our mouse
	// GLFW_CURSOR_NORMAL GLFW_CURSOR_DISABLED make a func
	// TODO(CK): Need a mode that will disable the camera control for the gui
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD\n";
		return EXIT_FAILURE;
	}

	// NOTE: OpenGL error checks have been omitted for brevity
	// OpenGL Configurations - after loading glad -
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	game->Init(*window);
	gui->Init(*window);
	guiState state;

	while (!glfwWindowShouldClose(window))
	{
		// NOTE(CK): Gamepad
		if (glfwJoystickPresent(GLFW_JOYSTICK_1))
		{
			int axesCount;
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
			//std::cout << axesCount << "\n";

			int buttonCount;
			const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
			if (GLFW_PRESS == buttons[1])
			{
				std::cout << "b button pressed" << "\n";
			}
			// gives you type (xbox)
			const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
			//std::cout << "Joystick is: " << name << "\n";
		}

		glfwPollEvents();

		// delta time
		float currentFrame = (float)glfwGetTime();
		dt.time = currentFrame - dt.lastFrame;
		dt.lastFrame = currentFrame;

		// Process the input & move the player
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // blue 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		game->ProcessCamera(dt.time);
		game->Render();

		state.gameDeltaTime = dt.time; // probably a better way to do this
		gui->SetState(state);
		gui->Update();
		gui->Render();
		glfwSwapBuffers(window);
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);

}

void processInput(GLFWwindow* window, int key, int action, int scancode, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }
	// NOTE(CK): includes camera input
	game->ProcessInput(key, action, scancode, mods, dt.time);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

	lastX = (float)xpos;
	lastY = (float)ypos;
	if (game->mode == Mode::CAMERA)
	{
		game->camera->ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//	game->camera->ProcessMouseScroll((float)yoffset);
//}

void gamepad_callback(int jid, int event)
{
	if (event == GLFW_CONNECTED)
	{
		// The joystick was connected
	}
	else if (event == GLFW_DISCONNECTED)
	{
		// The joystick was disconnected
	}
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}
