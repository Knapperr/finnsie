/*
	TODO:
		DEC 10 2019
		- [ ] Change Texture and Shader back to less object oriented code. 
			  These don't need to be objects they are simply structs that have functions 
			  run on them because they are data
		- [ ] Pass the input to the games update function
		- [ ] Not everything needs to be a class but some things are okay
			  Game is much easier to code as a class same with ResourceManager

		- [ ] Vertices manager VAO manager?
		- [ ] read up on InitRenderTextureData 
*/
#include "global.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "camera.h"
#include "renderer.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "game.h"


#include "render_helpers.h"

using namespace finnsie;

#define PI32 3.14159265359f
#define internal static 
#define local_persist static 
#define global_variable static

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

void processInput(GLFWwindow* window);

const unsigned int SCREEN_WIDTH = 1080;
const unsigned int SCREEN_HEIGHT = 720;

// NOTE: 2D has its own game for now
Game* game;
ResourceManager* g_resourceManager;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

float g_playerXPos = 0;
float g_velocity = 300.0f;

// Camera
glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0, +4.0f);

static void error_callback(int error, const char* description);
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// NOTE: INPUT FOR 2D
// Pass input to our game object
//bool g_keys[1024];
//bool g_keysProcessed[1024];
//void ProcessInput(float dt)
//{
//	// Calculate delta time before adding velocity
//	float velocity = ::g_velocity * dt;
//
//	if (g_keys[GLFW_KEY_A])
//	{
//		//enemy.pos.x -= velocity;
//	}
//	if (g_keys[GLFW_KEY_D])
//	{
//		//enemy.pos.x += velocity;
//	}
//	if (g_keys[GLFW_KEY_W])
//	{
//		//enemy.pos.y -= velocity;
//	}
//	if (g_keys[GLFW_KEY_S])
//	{
//		//enemy.pos.y += velocity;
//	}
//}

struct DeltaTime 
{
	float time;
	float lastFrame; 
};
global_variable DeltaTime dt;

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
	//glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	// tell GLFW to capture our mouse
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


	// NOTE: FOR 2D
	// Init game 
	// -------------------------------------------------------------------------- 
	game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT, window);
	game->Init();

	
	::g_resourceManager = new ResourceManager();
	
	// Init the cube with texture
	// --------------------------
	Shader shader = ::g_resourceManager->GenerateShader(001, "vert_text.glsl", "frag_text.glsl", NULL);

	std::vector<float> vertices;
	if (!LoadVertices(vertices, "texturevert.txt"))
	{
		std::cout << "Failed to load vertices\n";
		return EXIT_FAILURE;
	}
	unsigned int VBO, VAO;
	InitRenderTextureData(vertices, shader.id, VBO, VAO);

	vertices.clear();
	if (!LoadVertices(vertices, "basic_lighting_vertices.txt"))
	{
		std::cout << "Failed to load vertices\n";
		return EXIT_FAILURE;
	}
	// Init the light
	Shader lightShader = ::g_resourceManager->GenerateShader(002, "vert_lamp.glsl", "frag_lamp.glsl", NULL);
	// cant use the same VAO and VBO
	unsigned int lightVAO, lightVBO;
	InitLightData(vertices, lightVAO, lightVBO);


	// -------------------------------------------------------
	// I do this outside because i dont want to be calling getuniformlocation in the game loop
	int projLoc = glGetUniformLocation(shader.id, "projection");
	int viewLoc = glGetUniformLocation(shader.id, "view");
	int modelLoc = glGetUniformLocation(shader.id, "model");

	glm::vec3 lampPos = glm::vec3(2.2f, 1.0f, 2.0f);
	float lampXMove = 0.1f;
	int lightProjLoc = glGetUniformLocation(lightShader.id, "projection");
	int lightViewLoc = glGetUniformLocation(lightShader.id, "view");

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// delta time
		float currentFrame = (float)glfwGetTime();
		dt.time = currentFrame - dt.lastFrame;
		dt.lastFrame = currentFrame;

		// Process the input & move the player
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// NOTE: FOR 3D
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
											   (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
											    0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		// lamp
		glUseProgram(lightShader.id);
		glUniformMatrix4fv(lightProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(view));
		DrawLight(lightShader.id, lightVAO, lampPos);

		lampPos.x -= lampXMove * dt.time;
		if (lampPos.x < -1.8f)
		{
			lampXMove = -1.0f;
		}
		else if (lampPos.x > 1.8f)
		{
			lampXMove = 1.0f;
		}

		// Cube with Texture
		// -----------------------------------------
		glUseProgram(shader.id);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// This is where drawing of the cube starts <-- place in function
		glBindVertexArray(VAO);
		glm::mat4 model = glm::mat4(1.0f); // initialize matrix to indentity matrix first!
		//model = glm::translate(model, cubePositions[i]);
		// NOTE: modelLoc outside of the loop
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// NOTE: FOR 2D
		// DRAW
		//game->Render();

		glfwSwapBuffers(window);
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);

}

// Camera input
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, dt.time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, dt.time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, dt.time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, dt.time);
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GL_TRUE);
//
//	if (key >= 0 && key < 1024)
//	{
//		if (action == GLFW_PRESS)
//		{
//			::g_keys[key] = GL_TRUE;
//		}
//		else if (action == GLFW_RELEASE)
//		{
//			::g_keys[key] = GL_FALSE;
//			::g_keysProcessed[key] = GL_FALSE;
//		}
//	}
//}