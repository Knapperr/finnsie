#ifndef RENDERER_HG_
#define RENDERER_HG_

#include "global.h"
#include "shader_manager.h"
#include "camera.h"
#include "gui.h"
#include "uniform_manager.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace finnsie {

	class Renderer {
	public:
		Renderer();
		~Renderer();
		void BeginRender(Camera& cam);
		void DrawModel(Model& model);
		void DrawWater(Model& water, gui_state& state);
		void EndRender();
		void Shutdown();

	private:
		Shader modelShader;
		Shader normalShader;
		Shader waterShader;

		UniformManager* uniformManager;
		
		glm::vec3 lampPos;
		glm::vec3 lightPos;
		glm::vec3 camPos;
		glm::vec3 color;
		
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

		float colorChange;
		
		bool drawingNormals;

		void initUniforms();
		void initShaders();
		void startShader(unsigned int shaderId, int modelLoc, int projLoc, int viewLoc);
	};

}

#endif
