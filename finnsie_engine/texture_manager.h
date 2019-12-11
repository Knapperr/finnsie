#ifndef TEXTURE_MANAGER_HG_
#define TEXTURE_MANAGER_HG_

namespace finnsie {

	struct Texture
	{
		unsigned int id;
		unsigned int width;
		unsigned int height;
		unsigned int internalFormat;
		unsigned int imageFormat;

		// Texture configurations
		unsigned int wrapS;
		unsigned int wrapT;
		unsigned int filterMin;
		unsigned int filterMax;
	};

	void Generate(Texture* texture, unsigned int width, unsigned int height, unsigned char* data);
	void Bind(Texture* texture);

}
#endif