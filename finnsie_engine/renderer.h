#ifndef RENDERER_HG_
#define RENDERER_HG_

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace finnsie {

	class Renderer {
	// TODO: Currently the renderer 


	public:
		Renderer();
		~Renderer();
		void InitRenderCubeData();
		void DrawCube(unsigned int shaderId);

		void InitRenderLampData();
		void DrawLamp(unsigned int shaderId);
	private:
		unsigned int VBO;
		unsigned int cubeVAO;
		unsigned int lampVAO;
		glm::vec3 color;
	};

}

#endif
