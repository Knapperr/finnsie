#ifndef RESOURCE_MANAGER_HG_
#define RESOURCE_MANAGER_HG_


#include "texture_manager.h"
#include "shader_manager.h"
#include <map>


namespace finnsie {
	
	class ResourceManager
	{
	public:
		Texture GenerateTexture(int lookupId, const char* file, bool alpha);
		Shader GenerateShader(int lookupId, const char* vertName, const char* fragName, const char* geoName);

		Texture& GetTexture(int lookupId);
		Shader& GetShader(int lookupId);

		void ShutDown();

	private:
		std::map <int, Texture> textures;
		std::map <int, Shader> shaders;

		Texture loadTextureFromFile(const char* file, bool alpha);
	};


}

#endif
