#include "terrain.h"
#include <istream>

#include <stb_image/stb_image.h>
#include <fstream>
#include "log.h"

#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace finnsie {
	// Enable mutitexture blending across the terrain
	#ifndef ENABLE_MULTITEXTURE
	#define ENABLE_MULTITEXTURE 0
	#endif

	// Enable the blend constants based on the slope of the terrain
	#ifndef ENABLE_SLOPE_BASED_BLEND
	#define ENABLE_SLOPE_BASED_BLEND 0
	#endif 

	#define BUFFER_OFFSET(i) ((char*)NULL + (i))

	inline float GetPercentage(float value, const float min, const float max)
	{
		value = glm::clamp(value, min, max);
		return (value - min) / (max - min);
	}

	inline int GetFileLength(std::istream& file)
	{
		int pos = file.tellg();
		file.seekg(0, std::ios::end);
		int length = file.tellg();
		// Restore the position of the get pointer
		file.seekg(pos);
		return length;
	}

	inline void DeleteVertexBuffer(GLuint& vboID)
	{
		if (vboID != 0)
		{
			glDeleteBuffers(1, &vboID);
			vboID = 0;
		}
	}

	inline void CreateVertexBuffer(GLuint& vboID)
	{
		// Make sure we don't lose the reference to the previous VBO if there is one
		DeleteVertexBuffer(vboID);
		glGenBuffers(1, &vboID);
	}

	inline void DeleteTexture(GLuint* texID)
	{
		if (texID != 0)
		{
			glDeleteTextures(1, texID);
			texID = 0;
		}
	}

	inline void CreateTexture(GLuint* texID)
	{
		DeleteTexture(texID);
		glGenTextures(1, texID);
	}

	inline float GetHeightValue(const unsigned char* data, unsigned char numBytes)
	{
		switch (numBytes)
		{
			case 1:
			{
				return (unsigned char)(data[0]) / (float)0xff;
			}
			break;
			case 2:
			{
				return (unsigned short)(data[1] << 8 | data[0]) / (float)0xffff;
			}
			break;
			case 4:
			{
				return (unsigned int)(data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]) / (float)0xffffffff;
			}
			break;
			default:
			{
				assert(false);  // Height field with non standard pixle sizes
			}
			break;
		}

		return 0.0f;
	}

	Terrain::Terrain(float heightScale /* = 500.0f */, float blockScale /* = 2.0f */)
		: glVertexBuffer(0)
		, glNormalBuffer(0)
		, glColorBuffer(0)
		, glTex0Buffer(0)
		, glTex1Buffer(0)
		, glTex2Buffer(0)
		, glIndexBuffer(0)
		, localToWorldMatrix(1)
		, inverseLocalToWorldMatrix(1)
		, heightmapDimensions(0, 0)
		, heightScale(heightScale)
		, blockScale(blockScale)
	{
		memset(glTextures, 0, sizeof(glTextures));
	}

	Terrain::~Terrain()
	{
		Terminate();
	}

	unsigned int GenerateTexture(const char* filename)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format = GL_RGB;
			switch (nrComponents)
			{
			case 1:
				format = GL_ALPHA;
				break;
			case 3:
				format = GL_RGB;
				break;
			case 4:
				format = GL_RGBA;
				break;
			}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			
			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(data);
		}
		else
		{
			std::string msg = "Texture failed to load at path: " + std::string(filename);
			stbi_image_free(data);
		}
		return textureID;
	}

	bool Terrain::LoadTexture(const char* filename, unsigned int textureStage)
	{
		assert(textureStage < numTextures);
		// TODO(CK): I am calling gen textures twice because i call it in GenerateTexture as well
		DeleteTexture(&glTextures[textureStage]);

		// Prevent buffer overrun warning
		if (textureStage < numTextures)
		{
			glTextures[textureStage] = GenerateTexture(filename);
			return (glTextures[textureStage] != 0);
		}
	}
	
	void Terrain::Terminate()
	{
		DeleteVertexBuffer(glVertexBuffer);
		DeleteVertexBuffer(glNormalBuffer);
		DeleteVertexBuffer(glColorBuffer);
		DeleteVertexBuffer(glTex0Buffer);
		DeleteVertexBuffer(glTex1Buffer);
		DeleteVertexBuffer(glTex2Buffer);
		DeleteVertexBuffer(glIndexBuffer);

		for (unsigned int i = 0; i < numTextures; ++i)
		{
			DeleteTexture(&glTextures[i]);
		}
	}

	bool Terrain::LoadHeightmap(const char* filename, unsigned char bitsPerPixel, unsigned int width, unsigned int height)
	{
		//if (!std::exists(filename))
		//{
		//	std::cerr < "Could not find file: << filename
		//	return false;
		//}

		std::ifstream ifs; 
		ifs.open(filename, std::ifstream::binary);
		if (ifs.fail())
		{
			LOG("Failed to open file");
			return false;
		}

		const unsigned int bytesPerPixel = bitsPerPixel / 8;
		const unsigned int expectedFileSize = (bytesPerPixel * width * height);
		const unsigned int fileSize = GetFileLength(ifs);

		if (expectedFileSize != fileSize)
		{
			LOG("Expected file size differs from actual file size");
			return false;
		}

		// TODO(CK): Allocation????
		// STUDY(CK): Why are we doing this???
		/*
			I use an unsigned char array because I expect the values 
			contained in the height map to be in the range [0…255] for 
			an 8-bit height map and [0…65535] for a 16-bit height map.
		*/
		unsigned char* heightMap = new unsigned char[fileSize];
		ifs.read((char*)heightMap, fileSize);
		if (ifs.fail())
		{
			LOG("Error occurred while reading from the height map file");
			ifs.close();
			delete[] heightMap;
			return false;
		}
		ifs.close();

		unsigned int numVerts = width * height;
		positionBuffer.resize(numVerts);
		colorBuffer.resize(numVerts);
		normalBuffer.resize(numVerts);
		tex0Buffer.resize(numVerts);

		heightmapDimensions = glm::vec2(width, height);

		// size of the terrain in world units
		float terrainWidth = (width - 1) * blockScale;
		float terrainHeight = (height - 1) * blockScale;

		float halfTerrainWidth = terrainWidth * 0.5f;
		float halfTerrainHeight = terrainHeight * 0.5f;

		for (unsigned int j = 0; j < height; ++j)
		{
			for (unsigned int i = 0; i < width; ++i)
			{
				unsigned int index = (j * width) + i;
				assert(index * bytesPerPixel < fileSize);
				float heightValue = GetHeightValue(&heightMap[index * bytesPerPixel], bytesPerPixel);

				float S = (i / (float)(width - 1));
				float T = (j / (float)(height - 1));

				float X = (S * terrainWidth) - halfTerrainWidth;
				float Y = heightValue * heightScale;
				float Z = (T * terrainHeight) - halfTerrainHeight;

				// Blend 3 textures depending on the height of the terrain
				float tex0Contribution = 1.0f - GetPercentage(heightValue, 0.0f, 0.75f);
				float tex2Contribution = 1.0f - GetPercentage(heightValue, 0.75f, 1.0f);

				normalBuffer[index] = glm::vec3(0);
				positionBuffer[index] = glm::vec3(X, Y, Z);
#if ENABLE_MULTITEXTURE
				colorBuffer[index] = glm::vec4(tex0Contribution, tex0Contribution, tex0Contribution, tex2Contribution);
#else
				colorBuffer[index] = glm::vec4(1.0f);
#endif
				tex0Buffer[index] = glm::vec2(S, T);
			}
		}
		LOG("Terrain has been loaded!");
		delete[] heightMap;

		GenerateIndexBuffer();
		GenerateNormals();
		GenerateVertexBuffers();

		return true;
	}

	float Terrain::GetHeightAt(glm::vec3& pos)
	{
		return 0.0f;
	}

	void Terrain::GenerateIndexBuffer()
	{
		if (heightmapDimensions.x < 2 || heightmapDimensions.y < 2)
		{
			// Terrain hasn't been loaded, or is of an incorrect size
			return;
		}

		const unsigned int terrainWidth =  heightmapDimensions.x;
		const unsigned int terrainHeight = heightmapDimensions.y;

		// 2 triangles for every quad of the terrain mesh
		const unsigned int numTriangles = (terrainWidth - 1) * (terrainHeight - 1) * 2;

		// 3 indices for each triangle in the terrain mesh
		indexBuffer.resize(numTriangles * 3);

		unsigned int index = 0; // Index in the index buffer
		for (unsigned int j = 0; j < (terrainHeight - 1); ++j)
		{
			for (unsigned int i = 0; i < (terrainWidth - 1); ++i)
			{
				int vertexIndex = (j * terrainWidth) + i;
				// Top triangle (T0)
				indexBuffer[index++] = vertexIndex;                           // V0
				indexBuffer[index++] = vertexIndex + terrainWidth + 1;        // V3
				indexBuffer[index++] = vertexIndex + 1;                       // V1
				// Bottom triangle (T1)
				indexBuffer[index++] = vertexIndex;                           // V0
				indexBuffer[index++] = vertexIndex + terrainWidth;            // V2
				indexBuffer[index++] = vertexIndex + terrainWidth + 1;        // V3
			}
		}
	}

	void Terrain::GenerateNormals()
	{
		for (unsigned int i = 0; i < indexBuffer.size(); i += 3)
		{
			glm::vec3 v0 = positionBuffer[indexBuffer[i + 0]];
			glm::vec3 v1 = positionBuffer[indexBuffer[i + 1]];
			glm::vec3 v2 = positionBuffer[indexBuffer[i + 2]];

			glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

			normalBuffer[indexBuffer[i + 0]] += normal;
			normalBuffer[indexBuffer[i + 1]] += normal;
			normalBuffer[indexBuffer[i + 2]] += normal;
		}

		const glm::vec3 UP(0.0f, 1.0f, 0.0f);
		for (unsigned int i = 0; i < normalBuffer.size(); ++i)
		{
			normalBuffer[i] = glm::normalize(normalBuffer[i]);

#if ENABLE_SLOPE_BASED_BLEND
			//float fTexture0Contribution = glm::saturate(glm::dot(normalBuffer[i], UP) - 0.1f);
			//colorBuffer[i] = glm::vec4(fTexture0Contribution, fTexture0Contribution, fTexture0Contribution, colorBuffer[i].w);
#endif

		}
	}

	void Terrain::GenerateVertexBuffers()
	{
		// First generate the buffer object ID's
		CreateVertexBuffer(glVertexBuffer);
		CreateVertexBuffer(glNormalBuffer);
		CreateVertexBuffer(glColorBuffer);
		CreateVertexBuffer(glTex0Buffer);
		CreateVertexBuffer(glTex1Buffer);
		CreateVertexBuffer(glTex2Buffer);
		CreateVertexBuffer(glIndexBuffer);

		// Copy the host data into the vertex buffer objects
		glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positionBuffer.size(), &(positionBuffer[0]), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, glColorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colorBuffer.size(), &(colorBuffer[0]), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, glNormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normalBuffer.size(), &(normalBuffer[0]), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, glTex0Buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * tex0Buffer.size(), &(tex0Buffer[0]), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, glTex1Buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * tex0Buffer.size(), &(tex0Buffer[0]), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, glTex2Buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * tex0Buffer.size(), &(tex0Buffer[0]), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexBuffer.size(), &(indexBuffer[0]), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Terrain::RenderNormals()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(glm::value_ptr(localToWorldMatrix));

		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_LIGHTING);

		glColor3f(1.0f, 1.0f, 0.0f);// Yellow

		glBegin(GL_LINES);
		{
			for (unsigned int i = 0; i < positionBuffer.size(); ++i)
			{
				glm::vec3 p0 = positionBuffer[i];
				glm::vec3 p1 = (positionBuffer[i] + normalBuffer[i]);

				glVertex3fv(glm::value_ptr(p0));
				glVertex3fv(glm::value_ptr(p1));
			}
		}
		glEnd();

		glPopAttrib();

		glPopMatrix();
	}

	void Terrain::Render()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(glm::value_ptr(localToWorldMatrix));

		//
		// Texture Stage 0
		//
		// Simply output texture0 for stage 0.
		//
		glActiveTexture(GL_TEXTURE0);
		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glScalef(32.0f, 32.0f, 1.0f);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, glTextures[0]);

		glClientActiveTexture(GL_TEXTURE0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, glTex0Buffer);
		glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

#if ENABLE_MULTITEXTURE
		// Disable lighting because it changes the color of the vertices that are
		// used for the multitexture blending.
		glDisable(GL_LIGHTING);

		//
		// Texture Stage 1
		//
		// Perform a linear interpolation between the output of stage 0 
		// (i.e texture0) and texture1 and use the RGB portion of the vertex's 
		// color to mix the two. 
		//
		glActiveTexture(GL_TEXTURE1);
		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glScalef(32.0f, 32.0f, 1.0f);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, glTextures[1]);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);

		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);

		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PRIMARY_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);

		glClientActiveTexture(GL_TEXTURE1);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, glTex1Buffer);
		glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

		//
		// Texture Stage 2
		//
		// Perform a linear interpolation between the output of stage 1 
		// (i.e texture0 mixed with texture1) and texture2 and use the ALPHA 
		// portion of the vertex's color to mix the two. 
		//
		glActiveTexture(GL_TEXTURE2);
		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glScalef(32.0f, 32.0f, 1.0f);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, glTextures[2]);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);

		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);

		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PRIMARY_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);

		glClientActiveTexture(GL_TEXTURE2);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, glTex2Buffer);
		glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

#else
		glEnable(GL_TEXTURE);
		glEnable(GL_LIGHTING);
#endif

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
		glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, glColorBuffer);
		glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, glNormalBuffer);
		glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
		glDrawElements(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

#if ENABLE_MULTITEXTURE
		glActiveTexture(GL_TEXTURE2);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glClientActiveTexture(GL_TEXTURE2);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glActiveTexture(GL_TEXTURE1);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glClientActiveTexture(GL_TEXTURE1);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

		glActiveTexture(GL_TEXTURE0);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glClientActiveTexture(GL_TEXTURE0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

#if _DEBUG
		DebugRender();
#endif
	}


	void Terrain::DebugRender()
	{
		RenderNormals();
	}
}

