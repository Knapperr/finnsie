#ifndef GUI_HG_
#define GUI_HG_

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <stdio.h>

// include this after imgui
// you also need to include the files for imgui in here as well since its single header
#include "gl_common.h"
#include <vector>
#include <string>
#include <stb_image/stb_image.h>

namespace finnsie {

	struct model_info
	{
		int index = 0;
		float scale = 0.0f;
		bool viewNormals = false;
	};

	struct gui_state
	{
		model_info modelInfo;
		float active = false;
		float *cameraSpeed = 0; // set to the camera object's speed
		float gameDeltaTime;
	};

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
		void SetActive(bool active);
		void Update();
		void Render();
		void Shutdown();

		float playerVelocity;
		gui_state state;
	private:
		bool showDemoWindow = false;
		bool showAnotherWindow = false;

		std::vector<objFile> objPaths;
		void getFolders();
	};
}

#endif