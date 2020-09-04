#ifndef RESOURCE_MANAGER_HG_
#define RESOURCE_MANAGER_HG_


#include "texture_manager.h"
#include "shader_manager.h"
#include <map>


namespace finnsie {
	
	class ResourceManager
	{
    public:
		static Texture2D GenerateTexture(int lookupId, const char* file, bool alpha);
		static Shader GenerateShader(std::string lookup, const char* vertName, const char* fragName, const char* geoName);
        
		static Texture2D* GetTexture(int lookupId);
		static Shader* GetShader(std::string lookup);
        
		static void Shutdown();
        
    private:
		static std::map <int, Texture2D> textures;
		static std::map <std::string, Shader> shaders;
        
		static Texture2D loadTextureFromFile(const char* file, bool alpha);
	};
}

#endif
