#ifndef RESOURCE_MANAGER_HG_
#define RESOURCE_MANAGER_HG_


#include "texture_manager.h"
#include "shader_manager.h"
#include <map>


namespace finnsie {
	
	class ResourceManager
	{
        public:
		Texture2D GenerateTexture(int lookupId, const char* file, bool alpha);
		Shader GenerateShader(std::string lookup, const char* vertName, const char* fragName, const char* geoName);
        
		Texture2D& GetTexture(int lookupId);
		Shader& GetShader(std::string lookup);
        
		void ShutDown();
        
        private:
		std::map <int, Texture2D> textures;
		std::map <std::string, Shader> shaders;
        
		Texture2D loadTextureFromFile(const char* file, bool alpha);
	};
}

#endif
