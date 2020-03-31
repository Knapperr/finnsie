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

	struct gui_state
	{
		float gameDeltaTime;
		int modelIndex = 0;
		float modelScale = 0.0f;
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
		void Init(GLFWwindow& window);
		void SetState(gui_state state);
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