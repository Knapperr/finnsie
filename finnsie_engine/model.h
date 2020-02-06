#ifndef _MODEL_HG_
#define _MODEL_HG_

#include <vector>


// Model manager for cubes

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
	std::vector<float> vertices;
	bool LoadVertices(const char* vertFile);
	void createTexture(	unsigned int wrapS, unsigned int wrapT, unsigned int minFilter,
						unsigned int magFilter, unsigned int internalFormat, unsigned int imageFormat );
	// need to find a way to determine the position and texture coords not based off 
	void initTextureCubeData(unsigned int shaderId);
	void initBasicCubeData(unsigned int shaderId);
};

#endif