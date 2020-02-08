#ifndef _MODEL_HG_
#define _MODEL_HG_

#include <glm/vec3.hpp>
#include <vector>
namespace finnsie {

	struct Texture {
		unsigned int id;
		unsigned int width;
		unsigned int height;
		unsigned int wrapS;
		unsigned int wrapT;
		unsigned int minFilter;
		unsigned int magFilter;
		unsigned int internalFormat;
		unsigned int imageFormat;
	};

	// move this out
	class Model
	{
	public:
		unsigned int VAO;
		unsigned int VBO;
		Texture texture;
		glm::vec3 position;
		std::vector<float> vertices;
		bool LoadVertices(const char* vertFile);
		void CreateTexture(unsigned int wrapS, unsigned int wrapT, unsigned int minFilter,
			unsigned int magFilter, unsigned int internalFormat, unsigned int imageFormat);
		// need to find a way to determine the position and texture coords not based off 
		void InitTextureCubeData(unsigned int shaderId);
		void InitBasicCubeData(unsigned int shaderId);
	};
}
#endif