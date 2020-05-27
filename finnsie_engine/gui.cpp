#include "gui.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace finnsie {

	void Gui::Init(GLFWwindow& window, float& cameraSpeed)
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
		getFolders("objects");
		// could use this and then create a glm::vec4 from this field to change colour in the game
		//ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f); <-- can use a vec4 to control colours 


		// NOTE(CK): set the camera pointer
		this->state.cameraSpeed = &cameraSpeed;
	}

	bool Gui::Active()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse)
			return true;
		else 
			return false;
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


	void Gui::Update(draw_info& drawInfo)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
		if (showWaterWindow)
			waterWindow(&showWaterWindow, drawInfo);
		if (showModelWindow)
			modelWindow(&showModelWindow);

		// Show a simple window that we create ourselves. We use a begin/end pair to create a named window
		{
			// Only update the gui if its active
			ImGui::Begin("DEBUG MENU");
			ImGui::Checkbox("Demo Window", &showDemoWindow);
			ImGui::SameLine();
			ImGui::Checkbox("Water Window", &showWaterWindow);
			ImGui::SameLine();
			ImGui::Checkbox("Model Window", &showModelWindow);
			
			ImGui::SliderFloat("Camera Speed", this->state.cameraSpeed, 0.0f, 100.0f);


			ImGui::SliderFloat("Light X", &drawInfo.lightInfo.lightX, -200.0f, 400.0f);
			ImGui::SliderFloat("Light Y", &drawInfo.lightInfo.lightY, -200.0f, 400.0f);
			ImGui::SliderFloat("Light Z", &drawInfo.lightInfo.lightZ, -200.0f, 400.0f);
			ImGui::Separator();

			ImGui::End();
		}
	}

	void Gui::modelWindow(bool* p_open)
	{
		ImGui::Begin("Model Control", p_open);
		// Left pane
		static int selected = 0;
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		for (int i = 0; i < g_models.size(); i++)
		{
			char label[128];
			sprintf_s(label, "%s %d", g_models[i]->modelName.c_str(), i);
			if (ImGui::Selectable(label, selected == i))
			{
				selected = i;
			}
		}
		// TODO(CK): Create a better approach rather than creating pointers maybe
		// have an array of ints that mimics the size of g_models (more light weight)
		// or is creating an empty object fine!?!?
		ImGui::Separator();
		if (ImGui::Button("New Model"))
		{
			LOG("Creating empty model");
			CreateEmptyModel();
			LOG("Empty model created");
		}
		ImGui::EndChild();
		ImGui::SameLine();

		// Right pane models need to exist
		if (!g_models.empty())
		{
			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("%s", g_models[selected]->modelName.c_str());

			// Modal for loading meshes
			if (ImGui::Button("Load Mesh.."))
				ImGui::OpenPopup("Load Mesh");
			if (ImGui::BeginPopupModal("Load Mesh", NULL))
			{
				for (int i = 0; i < objPaths.size(); i++)
				{
					if (ImGui::SmallButton(objPaths[i].name.c_str()))
					{
						LOG("Loading mesh...");
						LoadEmptyModel(selected, objPaths[i].name, objPaths[i].path);
						LOG("mesh loaded!");
						ImGui::CloseCurrentPopup();
					}
				}

				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}

			ImGui::Separator();
			if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Controls"))
				{
					//ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
					ImGui::SliderFloat("scale", &g_models[selected]->scale, 0.0f, 30.0f);
					ImGui::Checkbox("show normals", &g_models[selected]->viewNormals);
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Details"))
				{
					ImGui::Text("ID: 0123456789");
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			if (ImGui::SmallButton("DELETE"))
			{
				UnloadModel(selected);
				// Selected is greater than size of vector
				// don't move down if empty
				if (selected >= g_models.size() && !g_models.empty())
					selected -= 1;
			}

			ImGui::EndChild();
			ImGui::EndGroup();
		}
		ImGui::End();
	}

	void Gui::waterWindow(bool* p_open, draw_info& drawInfo)
	{
		ImGui::Begin("WATER", p_open);

		ImGui::SliderFloat("U Jump", &drawInfo.waterInfo.uJump, 0.0f, 0.25f); // 0.25
		ImGui::SliderFloat("V Jump", &drawInfo.waterInfo.vJump, 0.0f, 0.25f);  // 0.25
		ImGui::Separator();
		ImGui::SliderFloat("Tiling", &drawInfo.waterInfo.tiling, 0.0f, 3.0f); // 3.0
		ImGui::SliderFloat("Speed", &drawInfo.waterInfo.speed, 0.0f, 5.0f); // 0.5
		ImGui::Separator();
		ImGui::SliderFloat("Flow Strength", &drawInfo.waterInfo.flowStrength, 0.0f, 1.0f); // 0.1
		ImGui::SliderFloat("Flow Offset", &drawInfo.waterInfo.flowOffset, -10.0f, 10.0f); // 0.0
		ImGui::Separator();
		ImGui::SliderFloat("Height Scale", &drawInfo.waterInfo.heightScale, 0.0f, 10.0f); // 0.1
		ImGui::SliderFloat("Height Scale Modulated", &drawInfo.waterInfo.heightScaleModulated, 0.0f, 20.0f); // 9.0
		ImGui::Separator();
		ImGui::Text("Directional Water ");
		ImGui::SliderFloat("gridResolution", &drawInfo.waterInfo.gridResolution, 0.0f, 40.0f); // 10.0
		ImGui::SliderFloat("TilingModulated", &drawInfo.waterInfo.tilingModulated, 0.0f, 80.0f); // 50
		ImGui::Checkbox("dualGrid", &drawInfo.waterInfo.dualGrid);

		ImGui::End();
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
	// Get folders based off of folder (objects , primitves) 
	void Gui::getFolders(std::string folder)
	{ 
		objFile obj = {};
		obj.loadModel = false;
		obj.loaded = false;
		std::string path = "content\\" + folder;
		for (auto& d : fs::recursive_directory_iterator(path))
		{
			if (d.path().extension() == ".obj")
			{
				obj.path = d.path().generic_string();
				obj.name = d.path().filename().generic_string();

				if (folder == "objects")
				{
					objPaths.push_back(obj);
				}
			}
		}
		// TODO(CK): Compare this with save file and set obj.loaded to true

	}
}