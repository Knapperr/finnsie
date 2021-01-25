#include "gui.h"
#include <iostream>
#include <filesystem>

#include "game.h"

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
        
		// Grab obj files in the folders
		getFolders("objects");
		// could use this and then create a glm::vec4 from this field to change colour in the game
		//ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f); <-- can use a vec4 to control colours 
	}
    	
	//
	// Main Menu
	//

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
		if (showTerrainWindow)
			terrainWindow(&showTerrainWindow);
		if (showPlayerWindow)
			playerWindow(&showPlayerWindow);
        
		// Show a simple window that we create ourselves. We use a begin/end pair to create a named window
		{
			// Only update the gui if its active
			ImGui::Begin("DEBUG");
			ImGui::Checkbox("Demo", &showDemoWindow);
			ImGui::SameLine();
			ImGui::Checkbox("Water", &showWaterWindow);
			ImGui::SameLine();
			ImGui::Checkbox("Model", &showModelWindow);
			ImGui::SameLine();
			ImGui::Checkbox("Terrain", &showTerrainWindow);
			ImGui::Checkbox("Player", &showPlayerWindow);
			ImGui::Separator();


			ImGui::Checkbox("Player Camera", &g_Game->followCameraActive);
			ImGui::SliderFloat("Camera Speed", &g_Game->camera->MovementSpeed, 0.0f, 100.0f);
            
			ImGui::SliderFloat("Light X", &g_lamp.x, -1500.0f, 1500.0f);
			ImGui::SliderFloat("Light Y", &g_lamp.y, -1500.0f, 1500.0f);
			ImGui::SliderFloat("Light Z", &g_lamp.z, -1500.0f, 1500.0f);
			ImGui::DragFloat("drag x", &g_lamp.x, 0.001f, -1500.0f, 2500.0f, "%.02f");
			ImGui::DragFloat("drag y", &g_lamp.y, 0.001f, -1500.0f, 2500.0f, "%.02f");
			ImGui::DragFloat("drag z", &g_lamp.z, 0.001f, -1500.0f, 2500.0f, "%.02f");

			ImGui::Separator();
                        
			ImGui::End();
		}
	}
    
	//
	// Model
	//

	void Gui::modelWindow(bool* p_open)
	{
		ImGui::Begin("Model Control", p_open);
		// Left pane
		static unsigned int selected = 0;
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		for (unsigned int i = 0; i < g_Game->objects.size(); i++)
		{
			char label[128];
			sprintf_s(label, "%s %d", g_Game->objects[i]->name.c_str(), i);
			if (ImGui::Selectable(label, selected == i))
			{
				selected = i;
			}
		}
		ImGui::Separator();
		if (ImGui::Button("New Model"))
		{
			CreateEmptyObject();
		}
		ImGui::EndChild();
		ImGui::SameLine();
        
		// Right pane models need to exist
		if (!g_Game->objects.empty())
		{
			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("%s", g_Game->objects[selected]->name.c_str());
            
			// Modal for loading meshes
			if (ImGui::Button("Load Mesh.."))
				ImGui::OpenPopup("Load Mesh");
			if (ImGui::BeginPopupModal("Load Mesh", NULL))
			{
				for (unsigned int i = 0; i < objPaths.size(); i++)
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
					ImGui::SliderFloat("scale", &g_Game->objects[selected]->scale, 0.0f, 200.0f);
					ImGui::DragFloat("fine scale", &g_Game->objects[selected]->scale, 0.0001f, 0.0f, 200.0f, "%.02f");
                    
					ImGui::DragFloat("x", &g_Game->objects[selected]->pos.x, 0.1f, -1000.0f, 1000.0f, "%.02f");
					ImGui::DragFloat("y", &g_Game->objects[selected]->pos.y, 0.1f, -1000.0f, 1000.0f, "%.02f");
					ImGui::DragFloat("z", &g_Game->objects[selected]->pos.z, 0.1f, -1000.0f, 1000.0f, "%.02f");
					
					ImGui::DragFloat("rot x", &g_Game->objects[selected]->orientation.x, 0.05f, -1000.0f, 1000.0f, "%.02f");
					ImGui::DragFloat("rot y", &g_Game->objects[selected]->orientation.y, 0.05f, -1000.0f, 1000.0f, "%.02f");
					ImGui::DragFloat("rot z", &g_Game->objects[selected]->orientation.z, 0.05f, -1000.0f, 1000.0f, "%.02f");

					ImGui::Checkbox("show normals", &g_Game->objects[selected]->viewNormals);
					ImGui::Checkbox("wire frame", &g_Game->objects[selected]->wireFrame);
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
				if (selected >= g_Game->objects.size() && !g_Game->objects.empty())
					selected -= 1;
			}
            
			ImGui::EndChild();
			ImGui::EndGroup();
		}
		ImGui::End();
	}
    
	//
	// Terrain
	//

	void Gui::terrainWindow(bool* p_open)
	{
		ImGui::Begin("Terrain", p_open);

		ImGui::Checkbox("wire frame", &g_Game->terrain->wireFrame);
		ImGui::Checkbox("draw texture", &g_Game->terrain->drawTexture);


		ImGui::SliderFloat("X", &g_Game->terrain->x, -1500.0f, 1500.0f);
		ImGui::SliderFloat("Z", &g_Game->terrain->z, -1500.0f, 1500.0f);

		ImGui::Separator();
		ImGui::Text("Grass");
		ImGui::Separator();
		ImGui::SliderInt("Grass Amount", &g_Game->terrain->grass.amount, 0, 100000);
		if (ImGui::SmallButton("Generate Grass")) { g_Game->terrain->GenerateGrass(); }
		ImGui::Separator();

		ImGui::Text("Texture");
		ImGui::Separator();
		if (ImGui::Button("UV")) { g_Game->terrain->selectedTextureId = g_Game->terrain->textureIds[0]; }
		ImGui::SameLine();
		if (ImGui::Button("Rock")) { g_Game->terrain->selectedTextureId = g_Game->terrain->textureIds[1]; }
		ImGui::SameLine();
		if (ImGui::Button("Grass")) { g_Game->terrain->selectedTextureId = g_Game->terrain->textureIds[2]; }
		ImGui::SameLine();
		if (ImGui::Button("Snow")) { g_Game->terrain->selectedTextureId = g_Game->terrain->textureIds[3]; }
		ImGui::Separator();

		ImGui::End();
	}

	//
	// Water
	//

	void Gui::waterWindow(bool* p_open)
	{
		ImGui::Begin("Water", p_open);
        
		static int e = 0;
		ImGui::RadioButton("distorted water", &e, 0); 
		ImGui::SameLine();
		ImGui::RadioButton("directional water", &e, 1);
        
		if (e == 0)
		{
			distortedWaterControls();
		}
        else if (e == 1)
        {
            directionalWaterControls();
        }
        
		ImGui::End();
	}
    
	void Gui::distortedWaterControls()
	{
        
		ImGui::DragFloat("ujump", &g_Game->distortWater->uJump,
                         0.001f, 0.0f, 0.25f, "%.02f");
		ImGui::DragFloat("vjump", &g_Game->distortWater->vJump,
                         0.001f, 0.0f, 0.25f, "%.02f");
		ImGui::Separator();
        
		ImGui::DragFloat("Tiling", &g_Game->distortWater->tiling,
                         0.1f, 0.0f, 10.0f, "%.01f"); // 3.0
		ImGui::DragFloat("Speed", &g_Game->distortWater->speed,
                         0.01f, 0.0f, 2.0f, "%.01f"); // 0.5
		ImGui::Separator();
        
		ImGui::DragFloat("Flow Strength", &g_Game->distortWater->flowStrength,
                         0.001f, 0.0f, 0.5f, "%.02f"); // 0.1
		ImGui::DragFloat("Flow Offset", &g_Game->distortWater->flowOffset,
                         0.001f, -1.5f, 2.0f, "%.02f");
		ImGui::Separator();
        
		ImGui::SliderFloat("Height Scale", &g_Game->distortWater->heightScale,
                           0.0f, 5.0f); // 0.1
		ImGui::SliderFloat("Height Scale Modulated", &g_Game->distortWater->heightScaleModulated,
                           0.0f, 20.0f); // 9.0
	}
    
    void Gui::directionalWaterControls()
    {
		ImGui::DragFloat("Tiling", &g_Game->dirWater->tiling,
                         0.1f, 0.0f, 10.0f, "%.01f"); // 3.0
		ImGui::DragFloat("Speed", &g_Game->dirWater->speed,
                         0.01f, 0.0f, 2.0f, "%.01f"); // 0.5
		ImGui::Separator();
        
		ImGui::DragFloat("Flow Strength", &g_Game->dirWater->flowStrength,
                         0.001f, 0.0f, 0.5f, "%.02f"); // 0.1
		ImGui::Separator();
        
		ImGui::SliderFloat("Height Scale", &g_Game->dirWater->heightScale,
                           0.0f, 5.0f); // 0.1
		ImGui::SliderFloat("Height Scale Modulated", &g_Game->dirWater->heightScaleModulated,
                           0.0f, 20.0f); // 9.0
        
		ImGui::SliderFloat("gridResolution", &g_Game->dirWater->gridResolution, 0.0f, 40.0f); // 10.0
		ImGui::SliderFloat("TilingModulated", &g_Game->dirWater->tilingModulated, 0.0f, 80.0f); // 50
		ImGui::Checkbox("dualGrid", &g_Game->dirWater->dualGrid);
    }
     
	//
	// Player
	//

	void Gui::playerWindow(bool* p_open)
	{
		ImGui::Begin("Player", p_open);

		ImGui::DragFloat("Speed", &g_Game->player->speed,
						 0.001f, 0.1f, 100.0f, "%.02f"); // 0.1
		ImGui::End();
	}

	//
	// Render 
	//

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
    
	bool Gui::Active()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse)
			return true;
		else
			return false;
	}

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