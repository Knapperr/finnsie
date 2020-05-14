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

	struct water_info
	{
		float uJump = 0.25f;
		float vJump = 0.25f;
		float tiling = 3.0f;
		float speed = 0.5f;
		float flowStrength = 0.07f;
		float flowOffset = -0.207f;
		float heightScale = 0.1f;
		float heightScaleModulated = 9.0f;
	};

	struct light_info
	{
		float lightX = -138.843f;
		float lightY = -19.247f;
		float lightZ = -14.226f;
	};

	struct gui_state
	{
		model_info modelInfo;
		water_info waterInfo;
		light_info lightInfo;
		float active = false;
		float *cameraSpeed = 0; // set to the camera object's speed
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
		void SetActive(bool active);
		void Update();
		void Render();
		void Shutdown();

		float playerVelocity;
		gui_state state;
	private:
		bool showWaterWindow = true;
		bool showModelWindow = false;

		
		bool showDemoWindow = false;
		bool showAnotherWindow = false;
		

		// TODO(CK): Remove replace with local function
		std::vector<objFile> objPaths;
		

		void waterWindow(bool* p_open);
		void modelWindow(bool* p_open);

		// TODO(CK): Remove replace with local function
		void getFolders(std::string folder);
	};
}

#endif