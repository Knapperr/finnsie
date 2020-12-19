#ifndef RENDERER_HG_
#define RENDERER_HG_

#include "camera.h"
#include "third_person_camera.h"
#include "gui.h"
#include "shader_manager.h"
#include "resource_manager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace finnsie {
	
	enum class RENDER_TYPE {
		MODEL,
		WATER,
		BINN,
		TERR,
	};

	class Renderer {
    public:
		Renderer();
		~Renderer();
		void BeginRender(float zoom, glm::mat4 viewMatrix, glm::vec3 position);
		void Draw(GameObject* obj);
		void DrawModel(GameObject& obj);
		void DrawWater(WaterObject* water);
		void DrawDirWater(WaterObject* dirWater);
        void DrawSphere(GameObject& obj);
		void DrawTerrain(Terrain* terr);
		void EndRender();
		void Shutdown();
        
	private:
		// Core shaders
		Shader* modelShader;
		Shader* waterShader;
		Shader* binnShader;
		Shader* dirWaterShader;
		Shader* lightShader;

		glm::vec3 lampPos;
		glm::vec3 lightPos;
		glm::vec3 camPos;
		
		glm::mat4 projection;
		glm::mat4 view;
        
		unsigned int VBO;
		unsigned int cubeVAO;
		unsigned int lampVAO;
        
		void drawGrass(Terrain* terr);
	};
    
}

#endif
