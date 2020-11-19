#include "gl_common.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "game.h"
#include "gui.h"

#include <windows.h>
#include <mmsystem.h> // timeBeginPerdod
#include <stdio.h>
#include <stdint.h>
#include <iostream>

using namespace finnsie;

void processInput(GLFWwindow* window, int key, int action, int scancode, int mods);

const unsigned int SCREEN_WIDTH = 1440;
const unsigned int SCREEN_HEIGHT = 900;

Game* finnsie::g_Game = NULL;
Input newInput = {};
Input oldInput = {};

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

int64_t g_perfCountFrequency;

struct Timer
{
	uint64_t diff;
	uint64_t milliseconds;
	double fixed_dt;
	double last;
	double elapsed;
	double previousElapsed;
};

global_variable Timer timer;

static void error_callback(int error, const char* description);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void gamepad_callback(int jid, int event);
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

inline LARGE_INTEGER Win32GetWallClock(void)
{
	LARGE_INTEGER result;
	QueryPerformanceCounter(&result);
	return result;
}

inline float Win32GetSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
	float result = ((float)(end.QuadPart - start.QuadPart) /
					(float)g_perfCountFrequency);
	return result;
}

inline void SwapInput()
{
	Input* temp = &newInput;
	newInput = oldInput;
	oldInput = *temp;
}

int main(int argc, char** argv)
{   
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// NOTE: Only works on major 3 and minor 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // NOTE(CK): This Turns off Vsync ???
    
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Finnsie", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	
	// NOTE(CK): This was set to 0 but it was using a ton of GPU and CPU 
	glfwSwapInterval(1); 

	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, processInput);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetJoystickCallback(gamepad_callback);
    
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD\n";
		return EXIT_FAILURE;
	}

	// Check OpenGL properties
	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));

	// TODO(CK): Look into this what error checks do i need to do?
	// NOTE: OpenGL error checks have been omitted for brevity
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	g_Game = new Game(*window);
	newInput = {};
	oldInput = {};

	// TODO(CK): Ill have to go through this update loop and 
	// really clean it up.

	// NOTE(tyler): this is for delta time averaging
	//		I know you can and should use a ring buffer for this, but I didn't want to include dependencies in this sample code
	// https://github.com/TylerGlaiel/FrameTimingControl/blob/master/frame_timer.cpp
	double updateRate = 60;
	int updateMultiplicity = 1;

	LARGE_INTEGER perfCountFrequencyResult;
	QueryPerformanceFrequency(&perfCountFrequencyResult);

	// fixed update
	timer.fixed_dt = 1.0 / updateRate;
	int64_t desiredFrameTime = perfCountFrequencyResult.QuadPart / (int64_t)updateRate;

	//these are to snap deltaTime to vsync values if it's close enough
	int64_t vsyncMaxError = perfCountFrequencyResult.QuadPart * .0002;
	int64_t time60hz = perfCountFrequencyResult.QuadPart / 60;
	int64_t snapFrequencies[] = { time60hz, // 60fps
								  time60hz * 2,      //30fps
								  time60hz * 3,      //20fps
								  time60hz * 4,      //15fps
								 (time60hz + 1) / 2,  //120fps //120hz, 240hz, or higher need to round up, so that adding 120hz twice guaranteed is at least the same as adding time_60hz once
								 // (time_60hz+2)/3,  //180fps //that's where the +1 and +2 come from in those equations
								 // (time_60hz+3)/4,  //240fps //I do not want to snap to anything higher than 120 in my engine, but I left the math in here anyway
	};

	const int timeHistoryCount = 4;
	int64_t timeAverager[timeHistoryCount] = { desiredFrameTime, desiredFrameTime, desiredFrameTime, desiredFrameTime };

	bool resync = true;
    
	// Get wall clock speed
	LARGE_INTEGER clock = Win32GetWallClock();
	int64_t prevFrameTime = clock.QuadPart;
	int64_t frameAccumulator = 0;
	//uint64_t lastCycleCount = __rdtsc();

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
        
		clock = Win32GetWallClock();
		int64_t currFrameTime = clock.QuadPart;
		int64_t deltaTime = currFrameTime - prevFrameTime;
		prevFrameTime = currFrameTime;
        
		//handle unexpected timer anomalies (overflow, extra slow frames, etc)
		if (deltaTime > desiredFrameTime * 8)
		{
			//ignore extra-slow frames
			deltaTime = desiredFrameTime;
		}
		if (deltaTime < 0)
		{
			deltaTime = 0;
		}

		//vsync time snapping
		for (int64_t snap : snapFrequencies)
		{
			if (std::abs(deltaTime - snap) < vsyncMaxError)
			{
				deltaTime = snap;
				break;
			}
		}

		// delta time averaging
		for (int i = 0; i < timeHistoryCount - 1; ++i)
		{
			timeAverager[i] = timeAverager[i + 1];
		}
		timeAverager[timeHistoryCount - 1] = deltaTime;
		deltaTime = 0;
		for (int i = 0; i < timeHistoryCount; i++)
		{
			deltaTime += timeAverager[i];
		}
		deltaTime /= timeHistoryCount;

		frameAccumulator += deltaTime;

		//spiral of death protection
		if (frameAccumulator > desiredFrameTime * 8)
		{
			resync = true;
		}

		// if requested 
		if (resync)
		{
			frameAccumulator = 0;
			deltaTime = desiredFrameTime;
			resync = false;
		}

		// Process the input & move the player
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // blue 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		while (frameAccumulator >= desiredFrameTime * updateMultiplicity)
		{
			for (int i = 0; i < updateMultiplicity; ++i)
			{
				// TODO(CK): Change double to float?
				g_Game->Update((float)timer.fixed_dt);
				frameAccumulator -= desiredFrameTime;
			}
		}

		g_Game->Render();
		g_Game->GuiUpdate();
		g_Game->GuiRender();
        
		glfwSwapBuffers(window);
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
	// TODO(CK): Process more inputs
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }

	newInput.controller.actionUp.endedDown      = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	newInput.controller.moveForward.endedDown   = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
	newInput.controller.moveBackward.endedDown  = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	newInput.controller.moveLeft.endedDown		= glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
	newInput.controller.moveRight.endedDown		= glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
	newInput.controller.moveUp.endedDown		= glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
	newInput.controller.moveDown.endedDown		= glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

	g_Game->ProcessInput(&newInput);
	SwapInput();
}

void processInput(GLFWwindow* window)
{
	// TODO(CK): Process more inputs
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }

	newInput.controller.actionUp.endedDown = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	newInput.controller.moveForward.endedDown = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
	newInput.controller.moveBackward.endedDown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	newInput.controller.moveLeft.endedDown = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
	newInput.controller.moveRight.endedDown = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
	newInput.controller.moveUp.endedDown = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
	newInput.controller.moveDown.endedDown = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

	g_Game->ProcessInput(&newInput);
	SwapInput();
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// TODO(CK): Process more inputs
	newInput.mouseButtons[0].endedDown = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
	newInput.mouseButtons[1].endedDown = button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS;
	g_Game->ProcessMouseButtons(&newInput);
	SwapInput();
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

	newInput.mouseX = xoffset;
	newInput.mouseY = yoffset;
	g_Game->ProcessMousePosition(&newInput);
	SwapInput();
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	newInput.xScrollOffset = (float)xoffset;
	newInput.yScrollOffset = (float)yoffset;
	g_Game->ProcessMouseScroll(&newInput);
	SwapInput();
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
	SwapInput();
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}