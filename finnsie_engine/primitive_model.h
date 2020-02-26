#ifndef _PRIMITIVE_MODEL_HG_
#define _PRIMITIVE_MODEL_HG_

#include "global.h"

#include <glm/vec3.hpp>
#include <vector>
#include <map>
#include <string>

namespace finnsie {

	struct ModelTexture {
		std::string name;
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
	class PrimitiveModel
	{
	public:
		unsigned int VAO;
		unsigned int VBO;
		std::map<std::string, ModelTexture> textures;
		glm::vec3 position;
		std::vector<float> vertices;
		bool LoadVertices(const char* vertFile);
		ModelTexture CreateTexture(std::string name, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter,
			unsigned int magFilter, unsigned int internalFormat, unsigned int imageFormat);
		// need to find a way to determine the position and texture coords not based off 
		void InitTextureCubeData(unsigned int shaderId);
		void InitTextureNormalCubeData(unsigned int shaderId);
		void InitBasicCubeData(unsigned int shaderId);

		void LoadTexture(ModelTexture& texture, unsigned int shaderId, const char* fileLocation, const char* uniformName, GLint value);

	};
}
#endif