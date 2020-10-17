#ifndef MODEL_HG_
#define MODEL_HG_

#include <glm/glm.hpp>	
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

namespace finnsie {
    
	typedef std::vector<Texture> texture_vec;
	typedef std::vector<Mesh> mesh_vec;
	typedef std::vector<Vertex> vertex_vec;
	typedef std::vector<unsigned int> uint_vec;
    
	class Model
	{
    public:
		texture_vec textures_loaded; 
		mesh_vec meshes;
		std::string directory;
		bool gammaCorrection;
        
		// NOTE(CK): For use with utility functions
		Model();
		Model(std::string const& path,
			  bool gamma = false);
        
    private:
		void loadModel(std::string const& path);

		// process a node in a recursive fashion. processes each individual mesh located at the node and repeats this process on its children nodes
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		
		// checks all material textures of a given type and loads the textures if they're not loaded yet.
		// the required info is returned as a Texture struct.
		texture_vec loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
		unsigned int TextureFromFile(const char* path, const std::string& dir, bool gamma = false);
	};
}
#endif