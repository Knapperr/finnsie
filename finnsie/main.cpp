#include "gl_common.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "game.h"
#include "gui.h"

#include <windows.h>
#include <mmsystem.h> // timeBeginPerdod TODO(CK): Look into this should be included with windows.h?
#include <stdio.h>
#include <stdint.h>
#include <iostream>

using namespace finnsie;

// TODO(CK): move to game
inline LARGE_INTEGER GetWallClock(void);
inline float GetSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end);
void processInput(GLFWwindow* window, int key, int action, int scancode, int mods);

const unsigned int SCREEN_WIDTH = 1440;
const unsigned int SCREEN_HEIGHT = 900;

Game* finnsie::g_Game = NULL;

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

int64_t g_perfCountFrequency;

struct Timer
{
	float dt;
	float lastFrame;
};
global_variable Timer timer;

static void error_callback(int error, const char* description);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void gamepad_callback(int jid, int event);
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(int argc, char** argv)
{
	// Delta time -- (Handmade Hero Day 573 as reference)
	LARGE_INTEGER perfCountFrequencyResult;
	QueryPerformanceFrequency(&perfCountFrequencyResult);
	g_perfCountFrequency = perfCountFrequencyResult.QuadPart;
    
	// TODO(CK): unresolved external for timeBeginPeriod?
	// NOTE(casey): Set the Windows scheduler granularity to 1ms
	// so that our Sleep() can be more granular.
	//UINT DesiredSchedulerMS = 1;
	//bool32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);
    
	//---------------------------------------------------------------------
	// glfw init and config
	// --------------------------------------------------------------------
	glfwSetErrorCallback(error_callback);
    
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// NOTE: Only works on major 3 and minor 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // TODO(CK): This Turns off Vsync ???
    
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Finnsie", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	
	// TODO(CK): Set this to 0 but it was using a ton of GPU and CPU 
	// This may be giving me WAY more consistent frames
	glfwSwapInterval(1); 

	glfwSetKeyCallback(window, processInput);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetJoystickCallback(gamepad_callback);
    
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD\n";
		return EXIT_FAILURE;
	}
	// Check OpenGL properties
	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));

	// NOTE: OpenGL error checks have been omitted for brevity
	// OpenGL Configurations - after loading glad -
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	g_Game = new Game(*window);
    
	// Get wall clock speed
	LARGE_INTEGER lastCounter = GetWallClock();
	uint64_t lastCycleCount = __rdtsc();
    
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
		timer.dt = currentFrame - timer.lastFrame;
		timer.lastFrame = currentFrame;
        
		// Process the input & move the player
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // blue 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		g_Game->Update(timer.dt);
		g_Game->Render();
        
		glfwSwapBuffers(window);
		
		// TODO(CK): Figure out where to put this
		LARGE_INTEGER endCounter = GetWallClock();
		double msPerFrame = 1000.0 * GetSecondsElapsed(lastCounter, endCounter);
		lastCounter = endCounter;
        
		// end cycles
		uint64_t endCycleCount = __rdtsc();
		uint64_t cyclesElapsed = endCycleCount - lastCycleCount;
		lastCycleCount = endCycleCount;
        
		double FPS = 0.0f;
		// mega cycles per frame
		double MCPF = (real64)(cyclesElapsed / (1000.0f * 1000.0f));
        
		// NOTE(CK): INCONSISTENT FRAMES LIKE HANDMADE]
		//printf("%.02fms/f %.02ff/s %.02fmc/f\n", msPerFrame, FPS, MCPF);
	}
	// NOTE(CK): CLEAN UP
	g_Game->Shutdown();
	delete g_Game;
	glfwDestroyWindow(window);
	glfwTerminate();
    
	exit(EXIT_SUCCESS);
    
}

void processInput(GLFWwindow* window, int key, int action, int scancode, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }
	g_Game->ProcessInput(key, action, scancode, mods, timer.dt);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	g_Game->ProcessMouseButtons(button, action, mods);
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
    
	// TODO(CK): Put this into the game class as well
	if (g_Game->leftMousePressed)
	{
		g_Game->camera->ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
// TODO(CK): Move callbacks to seperate file?
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!g_Game->gui->Active())
    {
        g_Game->camera->ProcessMouseScroll((float)yoffset);
    }
}

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

inline LARGE_INTEGER GetWallClock(void)
{
	LARGE_INTEGER result;
	QueryPerformanceCounter(&result);
	return result;
}

inline float GetSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
	float result = ((float)(end.QuadPart - start.QuadPart) /
                    (float)g_perfCountFrequency);
	return result;
}