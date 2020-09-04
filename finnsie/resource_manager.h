#ifndef RESOURCE_MANAGER_HG_
#define RESOURCE_MANAGER_HG_


#include "texture_manager.h"
#include "shader_manager.h"
#include <map>
#include <unordered_map>


namespace finnsie {
	
	class ResourceManager
	{
    public:
		static void Init();

		static Texture2D GenerateTexture(int lookupId, const char* file, bool alpha);
		static Shader GenerateShader(std::string lookup, const char* vertName, const char* fragName, const char* geoName);
        
		static Texture2D* GetTexture(int lookupId);
		static Shader* GetShader(std::string lookup);
        
		static void Shutdown();
        
    private:


		// TODO(CK):
		// Shaders have a lookup name, name and a group
		// the shader group determines what level 
		// we will have 
		// core shaders <-- everywhere uses these shaders
		// level shaders <-- these are broken down by level

		// TODO(CK):
		// Maybe get rid of this map and just have a pointer
		// to a resource struct that holds Shaders, Textures
		// and all kinds ofd
		static std::unordered_map <std::string, Shader> shaders;
		static std::unordered_map <int, Texture2D> textures;
        
		static Texture2D loadTextureFromFile(const char* file, bool alpha);
	};
}

#endif
