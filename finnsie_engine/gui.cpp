#include "gui.h"

#include "global.h"
#include "utils.h"


#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

namespace finnsie {

	void Gui::Init(GLFWwindow& window)
	{
		const char* glsl_version = "#version 330";

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

		// initialize memory
		state = {};
		// Grab obj files in the folders
		getFolders();
		// could use this and then create a glm::vec4 from this field to change colour in the game
		//ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f); <-- can use a vec4 to control colours 

	}

	void Gui::SetActive(bool active)
	{
		this->state.active = active;
	}

	/*
								Examples
		================================================================
		ImGui::Text("This is text");
		ImGui::SliderFloat("Player Velocity", &playerVelocity, 300.0f, 800.0f);
		ImGui::Text("application average %.3f ms/frame (%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Delta Time from game %.2f ms/frame", (double)state.gameDeltaTime * 1000.0f);

	    // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
        for (int i = 0; i < 7; i++)
        {
            if (i > 0)
                ImGui::SameLine();
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i/7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i/7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i/7.0f, 0.8f, 0.8f));
            ImGui::Button("Click");
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }

		// DROP DOWN COMBOBOX EXAMPLE
		static ImGuiComboFlags flags = 0;
		static const char* item_current = objPaths[0].name.c_str();            // Here our selection is a single pointer stored outside the object.
		static std::string item_current_path = objPaths[0].path.c_str();
		if (ImGui::BeginCombo("models", item_current, flags)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < objPaths.size(); n++)
			{
				bool is_selected = (item_current == objPaths[n].name.c_str());
				if (ImGui::Selectable(objPaths[n].name.c_str(), is_selected))
				{
					item_current = objPaths[n].name.c_str();
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
				}
			}
			ImGui::EndCombo();
		}
		=================================================================
	*/


	void Gui::Update()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		

		// deal with object loading checkboxes

		// This sample code is located in ImGui::ShowDemoWindow()
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);

		// Show a simple window that we create ourselves. We use a begin/end pair to create a named window
		{
			// Only update the gui if its active
			ImGui::Begin("DEBUG MENU");
			ImGui::Checkbox("Demo Window", &showDemoWindow);

			if (ImGui::CollapsingHeader("Models"))
			{
				for (int i = 0; i < objPaths.size(); i++)
				{
					ImGui::Checkbox(objPaths[i].name.c_str(), &objPaths[i].loadModel);

					// Load model do this once based off of loaded
					if (objPaths[i].loadModel && !objPaths[i].loaded)
					{
						LoadModel(objPaths[i].name, objPaths[i].path);
						objPaths[i].loaded = true;
					}
					if (!objPaths[i].loadModel && objPaths[i].loaded)
					{
						UnloadModel(objPaths[i].name);
						objPaths[i].loaded = false;
					}
				}
			}

			if (ImGui::CollapsingHeader("Model Edit"))
			{
				if (!g_models.empty())
				{
					ImGui::Text("Current model %s", g_models[state.modelInfo.index]->modelName.c_str());
					ImGui::SliderFloat("scale", &state.modelInfo.scale, 0.0f, 30.0f);
				}
			}

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.1f, 0.8f, 0.7f));
			if (ImGui::SmallButton("Save Models"))
			{

			}
			ImGui::PopStyleColor(1);

			ImGui::End();
			
		}


		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse)
		{
			state.active = true;
		}
		else
		{
			state.active = false;
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
		obj.loaded = false;
		std::string path = "content\\objects";
		for (auto& d : fs::recursive_directory_iterator(path))
		{
			if (d.path().extension() == ".obj")
			{
				obj.path = d.path().generic_string();
				obj.name = d.path().filename().generic_string();
				objPaths.push_back(obj);
			}
		}

		// TODO(CK): Compare this with save file and set obj.loaded to true

	}
}