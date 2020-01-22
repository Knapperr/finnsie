#ifndef RENDERER_HG_
#define RENDERER_HG_

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace finnsie {

	class Renderer {
	// TODO: Currently the renderer 


	public:
		Renderer();
		~Renderer();
		void InitRenderData();
		void DrawCube(unsigned int shaderId);
		void DrawLamp(unsigned int shaderId);
		bool LoadVertices(const char* vertices);
		bool LoadIndices(const char* indices);
		void InitRenderLampData();
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
