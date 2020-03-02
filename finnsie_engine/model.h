#ifndef MODEL_HG_
#define MODEL_HG_

#include "global.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader_manager.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

namespace finnsie {

	typedef std::vector<Texture> texture_vec;
	typedef std::vector<Mesh> mesh_vec;
	typedef std::vector<Vertex> vertex_vec;
	typedef std::vector<unsigned int> uint_vec;


	class Model
	{
	public:
		// NOTE(CK): stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once
		texture_vec textures_loaded; 
		mesh_vec meshes;
		std::string directory;
		std::string modelName;
		bool gammaCorrection;
		bool wireFrame;
		float scale;
		glm::vec3 pos;
		glm::vec3 orientation;

		/* Methods */
		Model(std::string modelName, 
			  bool wireFrame, 
			  glm::vec3 pos,
			  glm::vec3 orientation,
			  float scale,
			  std::string const& path,
			  bool gamma = false) : gammaCorrection(gamma)
		{
			this->wireFrame = wireFrame;
			this->pos = pos;
			this->orientation = orientation;
			this->scale = scale;
			this->modelName = modelName;
			loadModel(path);
		}

		void Draw(Shader shader)
		{
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].Draw(shader);
		}

	private:
		/* Methods */
		void loadModel(std::string const& path)
		{
			// read file via ASSIMP
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

			// check for errors
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is not zero
			{
				std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
				return;
			}

			// retrieve the directory path of the filepath
			directory = path.substr(0, path.find_last_of('/'));

			// process ASSIMP's root node recursively
			processNode(scene->mRootNode, scene);
		}

		// process a node in a recursive fashion. processes each individual mesh located at the node and repeats this process on its children nodes
		void processNode(aiNode *node, const aiScene *scene)
		{
			// process each mesh located at the current node
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				// the node object only contains indices to index the actual objects in the scene
				// the scene contains all the data, node is just to keep stuff organized (like relations between nodes)
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}

			// after we've processed all of the meshes we then recursivley process each of the child nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		}

		Mesh processMesh(aiMesh *mesh, const aiScene *scene)
		{
			// data to fill
			vertex_vec vertices;
			uint_vec indices;
			texture_vec textures;

			// walk through each of the mesh's vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				// we declare a placeholder vector since assimp uses its own vector class that doesn't convert glm's vec3 class 
				// so we transfer the data to a placeholder glm::vec3 first.
				glm::vec3 vector;
				// positions
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.position = vector;
				// normals
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.normal = vector;
				// texture coords
				if (mesh->mTextureCoords[0]) // does the mesh contain texture coords?
				{
					glm::vec2 vec;
					// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
					// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.texCoords = vec;
				}
				else
				{
					vertex.texCoords = glm::vec2(0.0f, 0.0f);
				}
				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.bitangent = vector;
				vertices.push_back(vertex);
			}
			// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				// retrieve all indices of the face and store them in the indices vector
				for (unsigned int j = 0; j < face.mNumIndices; j++)
				{
					indices.push_back(face.mIndices[j]);
				}
			}
			// process materials
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:
			// diffuse: texture_diffuseN
			// specular: texture_specularN
			// normal: texture_normalN

			// 1. diffuse maps
			texture_vec diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specualr maps
			texture_vec specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			texture_vec normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			// 4. height maps
			texture_vec heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			return Mesh(modelName, vertices, indices, textures);
			
		}

		// checks all material textures of a given type and loads the textures if they're not loaded yet.
		// the required info is returned as a Texture struct.
		texture_vec loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
		{
			texture_vec textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				bool skip = false;
				for (unsigned int j = 0; j < textures_loaded.size(); j++)
				{
					if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
					{
						textures.push_back(textures_loaded[j]);
						skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
						break;
					}
				}

				if (!skip)
				{
					Texture texture;
					texture.id = TextureFromFile(str.C_Str(), this->directory);
					texture.type = typeName;
					texture.path = str.C_Str();

					textures.push_back(texture);
					textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't load duplicate textures
				}
			}
			return textures;
		}

		unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false)
		{
			std::string filename = std::string(path);
			filename = directory + '/' + filename;

			unsigned int textureID;
			glGenTextures(1, &textureID);

			int width, height, nrComponents;
			unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

			if (data)
			{
				GLenum format;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 3)
					format = GL_RGB;
				else if (nrComponents == 4)
					format = GL_RGBA;

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				// unbind after using
				glBindTexture(GL_TEXTURE_2D, 0);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Texture failed to load at path: " << path << "\n";
				stbi_image_free(data);
			}
			return textureID;
		}
	};

}
#endif