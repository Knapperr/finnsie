#ifndef MESH_HG_
#define MESH_HG_

#include "global.h"
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
		std::string meshName;

		// constructor
		Mesh(std::string meshName,
			std::vector<Vertex> vertices,
			std::vector<unsigned int> indices,
			std::vector<Texture> textures)
		{
			this->meshName = meshName;
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			setupMesh();
		}

		void Draw(Shader shader)
		{
			unsigned int diffuseNr	= 1;
			unsigned int specularNr = 1;
			unsigned int normalNr	= 1;
			unsigned int heightNr	= 1;

			for (unsigned int i = 0; i < textures.size(); ++i)
			{
				glActiveTexture(GL_TEXTURE0 + i); // activate the proper texture unit before binding
				// retrieve texture number (the N in diffuse_TextureN)
				std::string number;
				std::string name = textures[i].type;

				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++);
				else if (name == "texture_height")
					number = std::to_string(heightNr++);

				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(shader.id, (name + number).c_str()), i);
				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
			}

			// Draw mesh
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// Always good practice to set everything back to defaults once configured
			// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

	private:
		unsigned int VBO, EBO;
		void setupMesh()
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			// Load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			// A great thing about structs is that their memory layout is sequential for all its items.
			// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
			// again translates to 3/2 floats which translates to a byte array.
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// Set the vertex attribute poiinters
			// Vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			// Vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

			// Vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

			// Vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

			// Vertex bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

			glBindVertexArray(0);
		}
	};
}
#endif