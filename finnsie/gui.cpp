#include "gui.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace finnsie {
    
	void Gui::Init(GLFWwindow& window, GameObject* book, float& cameraSpeed)
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
        this->guiBook = book;
        
        // TODO(CK): Refactor to better names?
        // also this page1path and dir stuff should just be figured out
        // inside the load texture file function....?
        std::string page1Path = "content/textures/jeff/cover.jpg";
        std::string page1Dir = page1Path.substr(0, page1Path.find_last_of('/'));
        bookTextures[0] = {};
        bookTextures[0].id = LoadTextureFile("cover.jpg", page1Dir, false);
        bookTextures[0].type = "texture_diffuse";
        bookTextures[0].path = page1Path;
        
        std::string page2Path = "content/textures/jeff/page1.jpg";
        std::string page2Dir = page2Path.substr(0, page2Path.find_last_of('/'));
        bookTextures[1] = {};
        bookTextures[1].id = LoadTextureFile("page1.jpg", page2Dir, false);
        bookTextures[1].type = "texture_diffuse";
        bookTextures[1].path = page2Path;
        
        std::string page3Path = "content/textures/jeff/page2.jpg";
        std::string page3Dir = page3Path.substr(0, page3Path.find_last_of('/'));
        bookTextures[2] = {};
        bookTextures[2].id = LoadTextureFile("page2.jpg", page3Dir, false);
        bookTextures[2].type = "texture_diffuse";
        bookTextures[2].path = page3Path;
        
        std::string page4Path = "content/textures/jeff/page3.jpg";
        std::string page4Dir = page4Path.substr(0, page4Path.find_last_of('/'));
        bookTextures[3] = {};
        bookTextures[3].id = LoadTextureFile("page3.jpg", page4Dir, false);
        bookTextures[3].type = "texture_diffuse";
        bookTextures[3].path = page4Path;
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
    
    
	void Gui::Update(WaterObject& disWater, WaterObject& dirWater)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
        
        
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
		if (showWaterWindow)
			waterWindow(&showWaterWindow, disWater, dirWater);
		if (showModelWindow)
			modelWindow(&showModelWindow);
        if (showBookWindow)
            bookWindow(&showBookWindow);
        
		// Show a simple window that we create ourselves. We use a begin/end pair to create a named window
		{
			// Only update the gui if its active
			ImGui::Begin("DEBUG MENU");
			ImGui::Checkbox("Demo Window", &showDemoWindow);
			ImGui::SameLine();
			ImGui::Checkbox("Water Window", &showWaterWindow);
			ImGui::SameLine();
			ImGui::Checkbox("Model Window", &showModelWindow);
            ImGui::Checkbox("Book Window", &showBookWindow);
			
			ImGui::SliderFloat("Camera Speed", this->state.cameraSpeed, 0.0f, 100.0f);
            
            
            // TODO(CK): LIGHTING MENU
			ImGui::SliderFloat("Light X", &g_lamp.x, -200.0f, 400.0f);
			ImGui::SliderFloat("Light Y", &g_lamp.y, -200.0f, 400.0f);
			ImGui::SliderFloat("Light Z", &g_lamp.z, -200.0f, 400.0f);
			ImGui::Separator();
            
			// TODO(CK): Remove book gui
            if (ImGui::SmallButton("Cover"))
            {
                this->guiBook->model->meshes[0].textures[0] = {};
                this->guiBook->model->meshes[0].textures[0] = bookTextures[0];
            }
            if (ImGui::SmallButton("Page 1"))
            {
                this->guiBook->model->meshes[0].textures[0] = {};
                this->guiBook->model->meshes[0].textures[0] = bookTextures[1];
            }
            if (ImGui::SmallButton("Page 2"))
            {
                this->guiBook->model->meshes[0].textures[0] = {};
                this->guiBook->model->meshes[0].textures[0] = bookTextures[2];
            }
            if (ImGui::SmallButton("Page 3"))
            {
                this->guiBook->model->meshes[0].textures[0] = {};
                this->guiBook->model->meshes[0].textures[0] = bookTextures[3];
            }
            
			ImGui::End();
		}
	}
    
	void Gui::modelWindow(bool* p_open)
	{
		ImGui::Begin("Model Control", p_open);
		// Left pane
		static int selected = 0;
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		for (int i = 0; i < g_objects.size(); i++)
		{
			char label[128];
			sprintf_s(label, "%s %d", g_objects[i]->name.c_str(), i);
			if (ImGui::Selectable(label, selected == i))
			{
				selected = i;
			}
		}
		// TODO(CK): Creating an empty object
		ImGui::Separator();
		if (ImGui::Button("New Model"))
		{
			CreateEmptyObject();
		}
		ImGui::EndChild();
		ImGui::SameLine();
        
		// Right pane models need to exist
		if (!g_objects.empty())
		{
			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("%s", g_objects[selected]->name.c_str());
            
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
						LoadEmptyObject(selected, objPaths[i].name, objPaths[i].path);
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
					ImGui::SliderFloat("scale", &g_objects[selected]->scale, 0.0f, 30.0f);
					ImGui::DragFloat("fine scale", &g_objects[selected]->scale, 0.0001f, 0.0f, 30.0f, "%.02f");
                    
					ImGui::DragFloat("x", &g_objects[selected]->pos.x, 0.001f, -1000.0f, 1000.0f, "%.02f");
					ImGui::DragFloat("y", &g_objects[selected]->pos.y, 0.001f, -1000.0f, 1000.0f, "%.02f");
					ImGui::DragFloat("z", &g_objects[selected]->pos.z, 0.001f, -1000.0f, 1000.0f, "%.02f");
					
					ImGui::Checkbox("show normals", &g_objects[selected]->viewNormals);
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
				UnloadObject(selected);
				// Selected is greater than size of vector
				// don't move down if empty
				if (selected >= g_objects.size() && !g_objects.empty())
					selected -= 1;
			}
            
			ImGui::EndChild();
			ImGui::EndGroup();
		}
		ImGui::End();
	}
    
	void Gui::waterWindow(bool* p_open, WaterObject& disWater, WaterObject& dirWater)
	{
		ImGui::Begin("WATER", p_open);
        
		static int e = 0;
		ImGui::RadioButton("distorted water", &e, 0); 
		ImGui::SameLine();
		ImGui::RadioButton("directional water", &e, 1);
        
		if (e == 0)
		{
			distortedWaterControls(disWater);
		}
        else if (e == 1)
        {
            directionalWaterControls(dirWater);
        }
        
		ImGui::End();
	}
    
	void Gui::distortedWaterControls(WaterObject& disWater)
	{
        
		ImGui::DragFloat("ujump", &disWater.uJump,
                         0.001f, 0.0f, 0.25f, "%.02f");
		ImGui::DragFloat("vjump", &disWater.vJump,
                         0.001f, 0.0f, 0.25f, "%.02f");
		ImGui::Separator();
        
		ImGui::DragFloat("Tiling", &disWater.tiling,
                         0.1f, 0.0f, 10.0f, "%.01f"); // 3.0
		ImGui::DragFloat("Speed", &disWater.speed,
                         0.01f, 0.0f, 2.0f, "%.01f"); // 0.5
		ImGui::Separator();
        
		ImGui::DragFloat("Flow Strength", &disWater.flowStrength,
                         0.001f, 0.0f, 0.5f, "%.02f"); // 0.1
		ImGui::DragFloat("Flow Offset", &disWater.flowOffset,
                         0.001f, -1.5f, 2.0f, "%.02f");
		ImGui::Separator();
        
		ImGui::SliderFloat("Height Scale", &disWater.heightScale,
                           0.0f, 5.0f); // 0.1
		ImGui::SliderFloat("Height Scale Modulated", &disWater.heightScaleModulated,
                           0.0f, 20.0f); // 9.0
	}
    
    void Gui::directionalWaterControls(WaterObject& dirWater)
    {
		ImGui::DragFloat("Tiling", &dirWater.tiling,
                         0.1f, 0.0f, 10.0f, "%.01f"); // 3.0
		ImGui::DragFloat("Speed", &dirWater.speed,
                         0.01f, 0.0f, 2.0f, "%.01f"); // 0.5
		ImGui::Separator();
        
		ImGui::DragFloat("Flow Strength", &dirWater.flowStrength,
                         0.001f, 0.0f, 0.5f, "%.02f"); // 0.1
		ImGui::Separator();
        
		ImGui::SliderFloat("Height Scale", &dirWater.heightScale,
                           0.0f, 5.0f); // 0.1
		ImGui::SliderFloat("Height Scale Modulated", &dirWater.heightScaleModulated,
                           0.0f, 20.0f); // 9.0
        
		ImGui::SliderFloat("gridResolution", &dirWater.gridResolution, 0.0f, 40.0f); // 10.0
		ImGui::SliderFloat("TilingModulated", &dirWater.tilingModulated, 0.0f, 80.0f); // 50
		ImGui::Checkbox("dualGrid", &dirWater.dualGrid);
    }
    
    void Gui::bookWindow(bool* p_open)
    {
        ImGui::Begin("Book", p_open);
        ImGui::SliderFloat("scale", &guiBook->scale, 0.0f, 30.0f);
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
	}
}