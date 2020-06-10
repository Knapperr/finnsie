#ifndef GUI_HG_
#define GUI_HG_

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <stdio.h>

/*	NOTE(CK):
	include this after imgui
	you also need to include the files for imgui in here as well since its single header 
*/
#include "gl_common.h"
#include <vector>
#include <string>
#include <stb_image/stb_image.h>

#include "utils.h"

namespace finnsie {
	// forward decl
	struct draw_info;

	struct gui_state
	{
		float *cameraSpeed = 0; // TODO(CK): JUST UPDATE DIRECTLY set to the camera object's speed
		float gameDeltaTime;
	};

	// TODO(CK): Remove replace with local function
	struct objFile
	{
		std::string path;
		std::string name;
		bool loadModel;
		bool loaded;
	};

	class Gui
	{
	public:
		void Init(GLFWwindow& window, float& cameraSpeed);
		void Update(draw_info& drawInfo);
		bool Active();
		void Render();
		void Shutdown();

		float playerVelocity;
		gui_state state;
	private:
		bool showDemoWindow = false;
		bool showWaterWindow = false;
		bool showModelWindow = false;
		bool showAnotherWindow = false;
		
		// TODO(CK): Remove replace with local function
		std::vector<objFile> objPaths;
		
		void waterWindow(bool* p_open, draw_info& drawInfo);
		void modelWindow(bool* p_open);
		// TODO(CK): Remove replace with local function
		void getFolders(std::string folder);
		
		void distortedWaterControls(draw_info& drawInfo);
	};
}

#endif