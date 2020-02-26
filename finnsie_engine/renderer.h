#ifndef RENDERER_HG_
#define RENDERER_HG_

#include "primitive_model.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace finnsie {

	class Renderer {
	public:
		Renderer();
		~Renderer();
		void DrawCube(unsigned int shaderId);

		void DrawTextureCube(unsigned int shaderId, PrimitiveModel textureCube, glm::vec3 cubePositions[],
							 int projLoc, int viewLoc, int modelLoc, glm::mat4 projection, glm::mat4 view);
		void DrawLamp(unsigned int shaderId, PrimitiveModel lightCube, unsigned int lightModelLoc,
					  unsigned int lightProjLoc, unsigned int lightViewLoc, glm::mat4 projection, 
					  glm::mat4 view, glm::vec3 lampPos);
		void DrawTextureNormalCube(unsigned int shaderId, PrimitiveModel textureCube, glm::vec3 cubePositions[], int projLoc,
								   int viewLoc, int modelLoc, glm::mat4 projection, glm::mat4 view, 
								   glm::vec3 cameraPos, glm::vec3 lightPos, unsigned int specularMap, unsigned int diffuseMap);
	private:
		std::vector<float> verticesVector;
		std::vector<float> indicesVector;
		unsigned int VBO;
		unsigned int cubeVAO;
		unsigned int lampVAO;

		glm::vec3 lampPos;
		glm::vec3 color;
		float colorChange;
	};

}

#endif
