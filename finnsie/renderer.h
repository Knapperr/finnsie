#ifndef RENDERER_HG_
#define RENDERER_HG_

#include "camera.h"
#include "gui.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader_manager.h"

namespace finnsie {
    
	class Renderer {
        public:
		Renderer();
		~Renderer();
		void BeginRender(Camera& cam);
		void DrawModel(GameObject& obj, Shader modelShader, Shader normalShader);
		void DrawWater(WaterObject* water, Shader waterShader);
		void DrawDirWater(WaterObject* dirWater, Shader waterShader);
        void DrawSphere(GameObject& obj, Shader binnShader);
		void EndRender();
		void Shutdown();
        
        private:        
		glm::vec3 lampPos;
		glm::vec3 lightPos;
		glm::vec3 camPos;
		
		glm::mat4 projection;
		glm::mat4 view;
        
		unsigned int VBO;
		unsigned int cubeVAO;
		unsigned int lampVAO;
        
		unsigned int activeModelShaderId;
		int activeModelLoc;
        
		bool drawingNormals;
        
		void startShader(unsigned int shaderId, int modelLoc, int projLoc, int viewLoc);
	};
    
}

#endif
