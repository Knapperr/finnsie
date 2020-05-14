#include "gui.h"
#include "global.h"
#include "utils.h"
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


		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
		if (showWaterWindow)
			waterWindow(&showWaterWindow);
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
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Current model %s", g_models[state.modelInfo.index]->modelName.c_str());
					ImGui::SameLine();

					// Arrow buttons with Repeater
					float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
					ImGui::PushButtonRepeat(true);
					if (ImGui::ArrowButton("##left", ImGuiDir_Left))
					{
						if (state.modelInfo.index > 0)
						{
							state.modelInfo.index--;
							// TODO(CK): Need to set the modelInfo to 
							// the current scale and viewNormals of the object
						}
					}
					ImGui::SameLine(0.0f, spacing);
					if (ImGui::ArrowButton("##right", ImGuiDir_Right))
					{
						if (state.modelInfo.index < g_models.size() - 1)
						{
							state.modelInfo.index++;
							// TODO(CK): Need to set the modelInfo to 
							// the current scale and viewNormals of the object
						}
					}
					ImGui::PopButtonRepeat();
					ImGui::SameLine();

					ImGui::Separator();

					// Options on models
					ImGui::SliderFloat("scale", &state.modelInfo.scale, 0.0f, 30.0f);
					ImGui::Checkbox("show normals", &state.modelInfo.viewNormals);
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.1f, 0.8f, 0.7f));
					if (ImGui::SmallButton("Apply texture"))
					{
						LoadTexture(state.modelInfo.index);
					}
					ImGui::PopStyleColor(1);
				}
			}


			ImGui::SliderFloat("Light X", &state.lightInfo.lightX, -200.0f, 200.0f);
			ImGui::SliderFloat("Light Y", &state.lightInfo.lightY, -200.0f, 200.0f);
			ImGui::SliderFloat("Light Z", &state.lightInfo.lightZ, -200.0f, 200.0f);
			ImGui::Separator();

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

	void Gui::modelWindow(bool* p_open)
	{
		ImGui::Begin("Example: Simple layout", p_open);
		//if (ImGui::BeginMenuBar())
		//{
		//	if (ImGui::BeginMenu("File"))
		//	{
		//		if (ImGui::MenuItem("Close")) *p_open = false;
		//		ImGui::EndMenu();
		//	}
		//	ImGui::EndMenuBar();
		//}

		// left pane
		static int selected = 0;
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		for (int i = 0; i < g_models.size(); i++)
		{
			char label[128];
			sprintf_s(label, "Model %d", i);
			if (ImGui::Selectable(label, selected == i))
				selected = i;
		}
		ImGui::EndChild();
		ImGui::SameLine();

		// right
		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
		ImGui::Text("MyObject: %d", selected);
		ImGui::Separator();
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Description"))
			{
				ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Details"))
			{
				ImGui::Text("ID: 0123456789");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();

		// TODO(CK): Create a better approach rather than creating pointers maybe
		// have an array of ints that mimics the size of g_models (more light weight)
		// or is creating an empty object fine!?!?
		if (ImGui::Button("New Model")) 
		{
			CreateEmptyModel();
		}
		
		ImGui::EndGroup();
		ImGui::End();
	}

	void Gui::waterWindow(bool* p_open)
	{
		ImGui::Begin("WATER", p_open);

		ImGui::SliderFloat("U Jump", &state.waterInfo.uJump, 0.0f, 0.25f); // 0.25
		ImGui::SliderFloat("V Jump", &state.waterInfo.vJump, 0.0f, 0.25f);  // 0.25
		ImGui::Separator();
		ImGui::SliderFloat("Tiling", &state.waterInfo.tiling, 0.0f, 3.0f); // 3.0
		ImGui::SliderFloat("Speed", &state.waterInfo.speed, 0.0f, 0.5f); // 0.5
		ImGui::Separator();
		ImGui::SliderFloat("Flow Strength", &state.waterInfo.flowStrength, 0.0f, 1.0f); // 0.1
		ImGui::SliderFloat("Flow Offset", &state.waterInfo.flowOffset, -10.0f, 10.0f); // 0.0
		ImGui::Separator();
		ImGui::SliderFloat("Height Scale", &state.waterInfo.heightScale, 0.0f, 0.1f); // 0.1
		ImGui::SliderFloat("Height Scale Modulated", &state.waterInfo.heightScaleModulated, 0.0f, 9.0f); // 9.0
		
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