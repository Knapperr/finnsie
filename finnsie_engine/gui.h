#ifndef GUI_HG_
#define GUI_HG_

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <stdio.h>

// include this after imgui
// you also need to include the files for imgui in here as well since its all files
#include "gl_common.h"
#include <vector>
#include <string>

namespace finnsie {

	struct guiState
	{
		float gameDeltaTime;
		
	};
	struct objFile
	{
		std::string path;
		std::string name;
		bool loadModel;
	};

	class Gui
	{
	public:
		void Init(GLFWwindow& window);
		void SetState(guiState state);
		void Update();
		void Render();
		void Shutdown();

		float playerVelocity;
	private:
		bool showDemoWindow = false;
		bool showAnotherWindow = false;
		guiState state;

		// method for finding folders objects
		bool loadModel = false;
		std::vector<objFile> objPaths;
		void getFolders();
	};
}

#endif