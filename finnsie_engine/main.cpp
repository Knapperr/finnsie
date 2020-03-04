#include "global.h"

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

using namespace finnsie;

void processCamera(GLFWwindow* window);
void processInput(GLFWwindow* window, int key, int action, int scancode, int mods);

const unsigned int SCREEN_WIDTH = 1080;
const unsigned int SCREEN_HEIGHT = 720;

// NOTE: 2D has its own game for now
Game* game;
ResourceManager* g_resourceManager;

// TODO(CK): Could have one camera with two views?
Camera debugCamera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera playerCamera(glm::vec3(0.0f, 2.0f, 2.0f));

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

float g_playerXPos = 0;
float g_velocity = 300.0f;

// Camera
glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0, +4.0f);
bool debugCameraActive = true;

static void error_callback(int error, const char* description);
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


struct DeltaTime 
{
	float time;
	float lastFrame; 
};
global_variable DeltaTime dt;

glm::vec3 lampPos = glm::vec3(2.2f, 1.0f, 2.0f);

std::vector<Model*> models;

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

// ----------------------------------------------------------------------------

	::g_resourceManager = new ResourceManager();
	Renderer renderer;

	// light model cube (no textures or else models with no textures load improperly)
	PrimitiveModel lightCube;
	std::vector<float> vertices;
	if (!lightCube.LoadVertices("basic_lighting_vertices.txt"))
	{
		std::cout << "Failed to load vertices\n";
		return EXIT_FAILURE;
	}
	// Init the light
	Shader lightShader = ::g_resourceManager->GenerateShader(001, "vert_lamp.glsl", "frag_lamp.glsl", NULL);
	lightCube.InitBasicCubeData(lightShader.id);
	glm::vec3 lightPos(1.2f, -0.5f, 0.5);
	// -------------------------------------------------------

	// Init the obj model
	Shader modelShader = ::g_resourceManager->GenerateShader(002, "vert_model.glsl", "frag_model.glsl", NULL);

	// TODO(CK): MOVE TO LOAD FUNCTION
	// -----------------------------------------
	Model* ourModel = new Model("donut", 
								false,
								glm::vec3(-20.0f, -3.75f, 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f),
								1.0f,
								"content/objects/donut/donutscaled.obj");
	models.push_back(ourModel);
	
	Model* nanoModel = new Model("nano",
								 false,
								 glm::vec3(-40.0f, -5.75f, 0.0f),
								 glm::vec3(0.0f, 2.0f, 0.0f),
								 0.2f,
								 "content/objects/nanosuit/nanosuit.obj");
	models.push_back(nanoModel);

	Model* rockModel = new Model("rock", 
								 false,
								 glm::vec3(-60.0f, -5.75f, 0.0f),
								 glm::vec3(0.0f, 0.0f, 0.0f),
								 0.5f, 
								 "content/objects/column/OldColumn.obj");
	models.push_back(rockModel);
	// ---------------------------------------------------

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float lampXMove = 0.1f;
	int lightProjLoc = glGetUniformLocation(lightShader.id, "projection");
	int lightViewLoc = glGetUniformLocation(lightShader.id, "view");
	int lightModelLoc = glGetUniformLocation(lightShader.id, "model");

	int objProjLoc = glGetUniformLocation(modelShader.id, "projection");
	int objViewLoc = glGetUniformLocation(modelShader.id, "view");
	int objModelLoc = glGetUniformLocation(modelShader.id, "model");

	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	while (!glfwWindowShouldClose(window))
	{
		processCamera(window);
		glfwPollEvents();

		// delta time
		float currentFrame = (float)glfwGetTime();
		dt.time = currentFrame - dt.lastFrame;
		dt.lastFrame = currentFrame;

		// Process the input & move the player
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // blue 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// TODO(CK): Put into camera class method
		if (debugCameraActive)
		{
			projection = glm::perspective(glm::radians(debugCamera.Zoom),
				(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
				0.1f, 100.0f);
			view = debugCamera.GetViewMatrix();
		}
		else
		{
			projection = glm::perspective(glm::radians(playerCamera.Zoom),
				(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
				0.1f, 100.0f);
			view = playerCamera.GetViewMatrix();
		}
		// lamp
		renderer.DrawLamp(lightShader.id, lightCube, lightModelLoc, 
						  lightProjLoc, lightViewLoc, projection, view, lampPos);

		// Move this position to the model class
		lampPos.x -= lampXMove * dt.time;
		if (lampPos.x < -1.8f)
		{
			lampXMove = -1.0f;
		}
		else if (lampPos.x > 1.8f)
		{
			lampXMove = 1.0f;
		}

		// render the model last
		glUseProgram(modelShader.id);
		// need to set the view and projection as well
		glUniformMatrix4fv(objProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(objViewLoc, 1, GL_FALSE, glm::value_ptr(view));

		for (unsigned int i = 0; i < models.size(); i++)
		{		
			renderer.DrawModel(*models[i], modelShader.id, objModelLoc);
		}

		glfwSwapBuffers(window);
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);

}

// Camera input
void processCamera(GLFWwindow* window)
{
	if (debugCameraActive)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { debugCamera.ProcessKeyboard(Camera_Movement::FORWARD, dt.time); }
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { debugCamera.ProcessKeyboard(Camera_Movement::BACKWARD, dt.time); }
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { debugCamera.ProcessKeyboard(Camera_Movement::LEFT, dt.time); }
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { debugCamera.ProcessKeyboard(Camera_Movement::RIGHT, dt.time); }
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { debugCamera.ProcessKeyboard(Camera_Movement::UP, dt.time); }
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { debugCamera.ProcessKeyboard(Camera_Movement::DOWN, dt.time); }
	}
	else
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { playerCamera.ProcessKeyboard(Camera_Movement::FORWARD, dt.time); }
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { playerCamera.ProcessKeyboard(Camera_Movement::BACKWARD, dt.time); }
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { playerCamera.ProcessKeyboard(Camera_Movement::LEFT, dt.time); }
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { playerCamera.ProcessKeyboard(Camera_Movement::RIGHT, dt.time); }
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { playerCamera.ProcessKeyboard(Camera_Movement::UP, dt.time); }
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { playerCamera.ProcessKeyboard(Camera_Movement::DOWN, dt.time); }
	}
}

void processInput(GLFWwindow* window, int key, int action, int scancode, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) { lampPos.z += 0.2f; }
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) { lampPos.z -= 0.2f; }

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) 
	{ 
		if (!debugCameraActive)
			debugCameraActive = true;
		else
			debugCameraActive = false;
	}

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

	debugCameraActive ? debugCamera.ProcessMouseMovement(xoffset, yoffset) : playerCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	debugCameraActive ? debugCamera.ProcessMouseScroll((float)yoffset) : playerCamera.ProcessMouseScroll((float)yoffset);
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}
