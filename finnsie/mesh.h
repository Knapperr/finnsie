#ifndef MESH_HG_
#define MESH_HG_

#include "shader_manager.h"
#include <glm/gtc/matrix_transform.hpp>

namespace finnsie {
    
	struct Vertex 
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};
    
	struct Texture
	{
		unsigned int id;
		std::string type;
		std::string path;
	};
    
	class Mesh
	{
    public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		unsigned int VAO;

		Mesh(std::vector<Vertex> vertices,
			 std::vector<unsigned int> indices,
			 std::vector<Texture> textures);
		void Draw(Shader shader);
        
     private:
		unsigned int VBO, EBO;
		void setupMesh();
	};
}
#endif