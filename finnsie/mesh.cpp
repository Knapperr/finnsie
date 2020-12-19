#include "mesh.h"

namespace finnsie {

	Mesh::Mesh(std::vector<Vertex> vertices,
			   std::vector<unsigned int> indices,
			   std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setupMesh();
	}

	void Mesh::setupMesh()
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

		/*
			NOTE(CK): IN GDP setting the vertexattrib pointers is done in a different way

			first the location is grabbed lets use the Texture coords as an example
				GLint vUV_location = glGetAttribPointer(shaderProgramID, "vUVx2"); <- vUVx2 is the same in the vertex/frag shader
			then we need to glEnableVertexAttribArray()
				glEnableVertexAttribArray(vUV_location); <- the location can be set manually instead of using [ layout (location = 2) in vec2 aTexCoords ]
			then we do the attribPointer
				glVertexAttribPointer(vUV_location, 4, GL_FLOAT, GL_FALSE,
										sizeof(sVertex), (void*)offsetof(sVertex, texCoords));

			This is just an example to show that we don't need to use the layout (location = 0) method that I am currently using
		*/

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
}