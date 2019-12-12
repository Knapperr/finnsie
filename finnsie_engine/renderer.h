#ifndef RENDERER_HG_
#define RENDERER_HG_

namespace finnsie {

	class Renderer {
	public:
		Renderer();
		~Renderer();
		void DrawCube(unsigned int shaderId);
	private:
		unsigned int cubeVAO;
		void initRenderCubeData();
		
	};

}

#endif
