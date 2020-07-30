#ifndef TERRAIN_H
#define TERRAIN_H

#include "gl_common.h"
#include <glm/glm.hpp>
#include <vector>

namespace finnsie {

    // Learning textured terrian with this tutorial
    // https://www.3dgep.com/multi-textured-terrain-in-opengl/
	class Terrain
	{
	public:
		Terrain(float heightScale = 500.0f, float blockScale = 2.0f);
		~Terrain();

		void Terminate();
		bool LoadHeightmap(const char* fileName, unsigned char bitsPerPixel, unsigned int width, unsigned int height);
		bool LoadTexture(const char* filename, unsigned int textureStage = 0);

		float GetHeightAt(glm::vec3& pos);

		void Render();
		void DebugRender();

		void GenerateIndexBuffer();
		void GenerateNormals();
		void GenerateVertexBuffers();
		void RenderNormals();

	private:
        typedef std::vector<glm::vec3>  PositionBuffer;
        typedef std::vector<glm::vec4>  ColorBuffer;
        typedef std::vector<glm::vec3>  NormalBuffer;
        typedef std::vector<glm::vec2>  TexCoordBuffer;
        typedef std::vector<GLuint>     IndexBuffer;

        PositionBuffer  positionBuffer;
        ColorBuffer     colorBuffer;
        NormalBuffer    normalBuffer;
        TexCoordBuffer  tex0Buffer;
        IndexBuffer     indexBuffer;

        // ID's for the VBO's
        GLuint  glVertexBuffer;
        GLuint  glNormalBuffer;
        GLuint  glColorBuffer;
        GLuint  glTex0Buffer;
        GLuint  glTex1Buffer;
        GLuint  glTex2Buffer;
        GLuint  glIndexBuffer;

        static const unsigned int numTextures = 3;
        unsigned int glTextures[numTextures];

        glm::mat4x4 localToWorldMatrix;

        // The dimensions of the heightmap texture
        glm::vec2 heightmapDimensions;
        glm::mat4x4 inverseLocalToWorldMatrix;

        // The height-map value will be multiplied by this value
        // before it is assigned to the vertex's Y-coordinate
        float   heightScale;
        // The vertex's X and Z coordinates will be multiplied by this
        // for each step when building the terrain
        float   blockScale;
	};

}
#endif