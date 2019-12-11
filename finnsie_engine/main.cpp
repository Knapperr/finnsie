
/*
	TODO:
		- [ ] Get the code cleaned up to this point

		- [ ] Create a Game Layer like handmade hero pass the input and memory to the game 
			  - want to keep this platform separated

		- [ ] Separate the systems. Pull process input into a game manager or class
		Don't just never use objects? just minimal C
		use more classes? (Not sure about malloc and free but also don't want 
						   to get too object oriented) Only use classes when 
						   necessary) C with a bit of C++?

		
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

// Renderer logic
//void InitRenderData(unsigned int *quadVAO)
//{
//	unsigned int VBO;
//	float vertices[] =
//	{
//		// Pos      // Tex
//		0.0f, 1.0f, 0.0f, 1.0f,
//		1.0f, 0.0f, 1.0f, 0.0f,
//		0.0f, 0.0f, 0.0f, 0.0f,																															
//
//		0.0f, 1.0f, 0.0f, 1.0f,
//		1.0f, 1.0f, 1.0f, 1.0f,
//		1.0f, 0.0f, 1.0f, 0.0f
//	};
//
//	glGenVertexArrays(1, quadVAO);
//	glGenBuffers(1, &VBO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//	glBindVertexArray(*quadVAO);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//}

// TODO: renderer.h
// Renderer logic
//void DrawGameObject(GameObject* gameObject, Shader* shader, glm::vec2 size,
//	float rotate, glm::vec3 color)
//void DrawGameObject(GameObject* gameObject, Shader *shader, unsigned int *quadVAO)
//{
//	// this takes in a sprite renderer
//	// renderer.DrawSprite(sprite, pos, size, rotation, color)
//	// this function is below
//	glUseProgram(shader->id);
//	glm::mat4 model = glm::mat4(1.0f);
//	model = glm::translate(model, glm::vec3(gameObject->pos, 0.0f));
//
//	model = glm::translate(model, glm::vec3(0.5f * gameObject->size.x, 0.0f * gameObject->size.y, 0.0f));
//	model = glm::rotate(model, gameObject->rotation, glm::vec3(0.0f, 0.0f, 1.0f));
//	model = glm::translate(model, glm::vec3(-0.5f * gameObject->size.x, -0.5f * gameObject->size.y, 0.0f));
//
//	model = glm::scale(model, glm::vec3(gameObject->size, 1.0f));
//
//	int modelLoc = glGetUniformLocation(shader->id, "model");
//	int spriteColorLoc = glGetUniformLocation(shader->id, "spriteColor");
//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//	glUniform3f(spriteColorLoc, gameObject->color.x, gameObject->color.y, gameObject->color.z);
//
//	//glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
//	//glUniform3f(glGetUniformLocation(shader->id, "spriteColor"), gameObject->color.x, gameObject->color.y, gameObject->color.z);
//
//	// Bind(&texture);
//	//Bind(&gameObject->sprite);
//	glBindTexture(GL_TEXTURE_2D, gameObject->sprite.id);
//
//
//	// After init the render data
//	glBindVertexArray(*quadVAO);
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//	glBindVertexArray(0);
//}

// TODO: object_manager.h
//GameObject enemy;


// TODO game.h
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

	// This is part of sprite_renderer
	// the renderer class will have this
	//unsigned int quadVAO;
	//InitRenderData(&quadVAO);

	//_renderer = new SpriteRenderer(g_resourceManager->GetShader("sprite"));
	//Room* firstRoom = new Room(this->Width, this->Height, "fileName.txt");
	//_rooms.push_back(firstRoom);



	// GenerateTexture is inside LoadTextureFromFile
	// NOTE(Cole): in the old code generateTexture on the resource manager is creating 
	// the texture callng loadFromFile then Generate is being called on the texture
	// it shoves it in a map after
	//Texture enemyTexture = LoadTextureFromFile("content/sprites/enemy.png", true); 
	//enemy = {};
	//enemy.name = "Enemy";
	//enemy.size = glm::vec2(64, 64);
	//enemy.pos = glm::vec2((SCREEN_WIDTH - 64) / 2, (SCREEN_HEIGHT - 64) / 2);
	//enemy.sprite = enemyTexture;
	//enemy.color = glm::vec3(1.0f, 1.0f, 1.0f);


	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// TODO: INIT GAME OBJ here!!


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