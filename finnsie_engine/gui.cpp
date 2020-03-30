#include "gui.h"

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

namespace finnsie {

	void Gui::Init(GLFWwindow& window)
	{

		// Player velocity
		playerVelocity = 300.0f;

		const char* glsl_version = "#version 130";

		// set up
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(&window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		showDemoWindow = false;
		showAnotherWindow = false;

		// Grab obj files in the folders
		getFolders();
		// could use this and then create a glm::vec4 from this field to change colour in the game
		//ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f); <-- can use a vec4 to control colours 

	}

	void Gui::SetState(guiState state)
	{
		this->state.gameDeltaTime = state.gameDeltaTime;
	}

	void Gui::Update()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// This sample code is located in ImGui::ShowDemoWindow()
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);

		// Show a simple window that we create ourselves. We use a begin/end pair to create a named window
		{
			ImGui::Begin("DEBUG MENU");

			ImGui::Text("This is text");
			ImGui::Checkbox("Demo Window", &showDemoWindow);
			ImGui::Checkbox("Show another window", &showAnotherWindow);

			ImGui::SliderFloat("Player Velocity", &playerVelocity, 300.0f, 800.0f);

			// checkbox for all models 
			for (int i = 0; i < objPaths.size(); ++i)
			{
				// how do we bind that that to a funcion
				ImGui::Checkbox(objPaths[i].name.c_str(), &objPaths[i].loadModel);

				// NOTE(CK):
				// call some function?
				// or can we get the state from this...
				// probably need to do a check but we don't want 
				// to poll this ALL the time......
			}

			ImGui::Text("application average %.3f ms/frame (%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Delta Time from game %.2f ms/frame", (double)state.gameDeltaTime * 1000.0f);
			ImGui::End();
		}

		// Show another simple window
		if (showAnotherWindow)
		{
			ImGui::Begin("Another window", &showAnotherWindow);
			ImGui::Text("Hey! Another window");
			if (ImGui::Button("Close"))
				showAnotherWindow = false;
			ImGui::End();
		}
	}

	void Gui::Render()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// for docking
		// must be called after rendering
		GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backupCurrentContext);

	}

	void Gui::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	// Run on init
	// TODO(CK): Get rid of C++17 nonsense
	void Gui::getFolders()
	{
		objFile obj = {};
		obj.loadModel = false;
		std::string path = "content\\objects";
		for (auto& d : fs::recursive_directory_iterator(path))
		{
			if (d.path().extension() == ".obj")
			{
				obj.path = d.path().string();
				obj.name = d.path().filename().string();
				objPaths.push_back(obj);
				std::cout << d.path().string() << "\n";
			}
		}
	}
}