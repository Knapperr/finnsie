#ifndef GUI_HG_
#define GUI_HG_

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <stdio.h>

/*	NOTE(CK):
	include this after imgui
	you also need to include the files for imgui in here as well since its single header 
*/
#include "gl_common.h"
#include <vector>
#include <string>
#include <stb_image/stb_image.h>

#include "utils.h"

namespace finnsie {
	// forward decl
	class WaterObject;
    class GameObject;
    
	struct gui_state
	{
		float *cameraSpeed = 0; // TODO(CK): JUST UPDATE DIRECTLY set to the camera object's speed
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
		void Update(WaterObject& disWater, WaterObject& dirWater);
		bool Active();
		void Render();
		void Shutdown();
        
		float playerVelocity;
		gui_state state;
        
        private:
		bool showDemoWindow = false;
		bool showWaterWindow = false;
		bool showModelWindow = false;
		bool showTerrainWindow = false;
		bool showAnotherWindow = false;
		
		// TODO(CK): Keep getting all of the folders on start up
		// make local if you want it to refetch the folders when opening it
		std::vector<objFile> objPaths;
		
		void terrainWindow(bool* p_open);
		void waterWindow(bool* p_open, WaterObject& disWater, WaterObject& dirWater);
		void modelWindow(bool* p_open);
		// TODO(CK): Remove replace with local function
		void getFolders(std::string folder);
		
		void distortedWaterControls(WaterObject& disWater);
        void directionalWaterControls(WaterObject& dirWater);
                

	};
}

#endif