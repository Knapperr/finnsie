
/*
	TODO:
		DEC 10 2019
		- [ ] Change Texture and Shader back to less object oriented code. 
			  These don't need to be objects they are simply structs that have functions 
			  run on them because they are data
		- [ ] Pass the input to the games update function
		- [ ] Not everything needs to be a class but some things are okay
			  Game is much easier to code as a class same with ResourceManager
*/
#include "global.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader_manager.h"
#include "texture_manager.h"
#include "game.h"


using namespace finnsie;

#define PI32 3.14159265359f

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

struct Fin_Vertex
{
	float x, y;
	float r, g, b;
};

const unsigned int SCREEN_WIDTH = 1080;
const unsigned int SCREEN_HEIGHT = 720;

Game* game;

float g_playerXPos = 0;
float g_velocity = 300.0f;

// Camera
glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0, +4.0f);

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Pass input to our game object
bool g_keys[1024];
bool g_keysProcessed[1024];
void ProcessInput(float dt)
{
	// Calculate delta time before adding velocity
	float velocity = ::g_velocity * dt;

	if (g_keys[GLFW_KEY_A])
	{
		//enemy.pos.x -= velocity;
	}
	if (g_keys[GLFW_KEY_D])
	{
		//enemy.pos.x += velocity;
	}
	if (g_keys[GLFW_KEY_W])
	{
		//enemy.pos.y -= velocity;
	}
	if (g_keys[GLFW_KEY_S])
	{
		//enemy.pos.y += velocity;
	}

}

int main(int argc, char** argv)
{
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
	glfwSetKeyCallback(window, key_callback);

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
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Init game 
	// --------------------------------------------------------------------------
	game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT, window);
	game->Init();

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// delta time
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Process the input & move the player
		ProcessInput(deltaTime);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// DRAW
		game->Render();

		glfwSwapBuffers(window);
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			::g_keys[key] = GL_TRUE;
		}
		else if (action == GLFW_RELEASE)
		{
			::g_keys[key] = GL_FALSE;
			::g_keysProcessed[key] = GL_FALSE;
		}
	}
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}