#ifndef RENDERER_HG_
#define RENDERER_HG_

#include "shader_manager.h"
#include "camera.h"
#include "gui.h"
#include "uniform_manager.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace finnsie {

	//struct water_info;
	//struct light_info;

	class Renderer {
	public:
		Renderer();
		~Renderer();
		void BeginRender(Camera& cam);
		void DrawModel(Model& model);
		void DrawWater(Model* water, draw_info& drawInfo);
		void DrawDirWater(Model& dirWater, draw_info& drawInfo);
		void EndRender();
		void Shutdown();

	private:
		Shader modelShader;
		Shader normalShader;
		Shader waterShader;
		Shader waterDirShader;

		UniformManager* uniformManager;
		
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
		int activeViewLoc;

		int objProjLoc;
		int objViewLoc;
		int objModelLoc;
		int normalProjLoc;
		int normalViewLoc;
		int normalModelLoc;
		int watProjLoc;
		int watViewLoc;
		int watModelLoc;
	
		bool drawingNormals;

		void initUniforms();
		void initShaders();
		void startShader(unsigned int shaderId, int modelLoc, int projLoc, int viewLoc);
	};

}

#endif
