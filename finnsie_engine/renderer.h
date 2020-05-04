#ifndef RENDERER_HG_
#define RENDERER_HG_

#include "primitive_model.h"
#include "shader_manager.h"
#include "model.h"
#include "camera.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace finnsie {

	class Renderer {
	public:
		Renderer();
		~Renderer();
		void BeginRender(Camera& cam);
		void DrawModel(Model& model);
		void DrawWater();
		void EndRender();
		void Shutdown();


		// Deprecated functions
		void DrawTextureCube(unsigned int shaderId, PrimitiveModel textureCube, glm::vec3 cubePositions[],
							 int projLoc, int viewLoc, int modelLoc, glm::mat4 projection, glm::mat4 view);
		void DrawLamp(unsigned int shaderId, PrimitiveModel lightCube, unsigned int lightModelLoc,
					  unsigned int lightProjLoc, unsigned int lightViewLoc, glm::mat4 projection, 
					  glm::mat4 view, glm::vec3 lampPos);
		void DrawTextureNormalCube(unsigned int shaderId, PrimitiveModel textureCube, glm::vec3 cubePositions[], int projLoc,
								   int viewLoc, int modelLoc, glm::mat4 projection, glm::mat4 view, 
								   glm::vec3 cameraPos, glm::vec3 lightPos, unsigned int specularMap, unsigned int diffuseMap);

	private:
		Shader modelShader;
		Shader normalShader;

		Model* waterQuad;

		std::vector<float> verticesVector;
		std::vector<float> indicesVector;
		
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

		float colorChange;
		
		bool drawingNormals;

		void initUniforms();
		void initShaders();
		void startShader(unsigned int shaderId, int modelLoc, int projLoc, int viewLoc);
	};

}

#endif
