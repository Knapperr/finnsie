#include "terrain.h"

#include "utils.h"
#include <string>
#include <glm/gtc/type_ptr.hpp>

#define SIZE 400
#define VERTEX_COUNT 128

namespace finnsie {

    Terrain::Terrain(int gridX, int gridZ)
    {
        this->x = (float)gridX * SIZE;
        this->z = (float)gridZ * SIZE;

        this->VAO = 0;
        this->VBO = 0;
        this->indicesLength = 0;
        this->textureId = 0;

        this->wireFrame = false;

        this->vertices = new FVertex[VERTEX_COUNT * VERTEX_COUNT];

        // Load a texture 
        std::string textPath = "content/textures/terr/grass.jpg";
        std::string textDir = textPath.substr(0, textPath.find_last_of('/'));
        textureId = LoadTextureFile("grass.jpg", textDir, false);
    }

    Terrain::~Terrain()
    {
        delete[] vertices;
        glDeleteVertexArrays(1, &this->VAO);
        glDeleteBuffers(1, &this->VBO);
        // TODO(CK): delete index buffer too?
    }

    void Terrain::Generate()
    {
        int* indices = new int[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1)];

        int verticesLength = VERTEX_COUNT * VERTEX_COUNT;
        this->indicesLength = 6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1);

        int index = 0;
        for (int i = 0; i < VERTEX_COUNT; ++i)
        {
            for (int j = 0; j < VERTEX_COUNT; ++j)
            {
                // Vertices
                this->vertices[index].position.x = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
                // TODO(CK): Keep track of these heights?
                this->vertices[index].position.y = 0;
                this->vertices[index].position.z = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;

                // Normals
                this->vertices[index].normal.x = 0;
                this->vertices[index].normal.y = 1;
                this->vertices[index].normal.z = 0;

                // Texture Coords
                this->vertices[index].texCoords.x = (float)j / ((float)VERTEX_COUNT - 1);
                this->vertices[index].texCoords.y = (float)i / ((float)VERTEX_COUNT - 1);
                ++index;
            }
        }
        index = 0;
        for (int gz = 0; gz < VERTEX_COUNT - 1; ++gz)
        {
            for (int gx = 0; gx < VERTEX_COUNT - 1; ++gx)
            {
                int topLeft = (gz * VERTEX_COUNT) + gx;
                int topRight = topLeft + 1;
                int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
                int bottomRight = bottomLeft + 1;

                indices[index++] = topLeft;
                indices[index++] = bottomLeft;
                indices[index++] = topRight;
                indices[index++] = topRight;
                indices[index++] = bottomLeft;
                indices[index++] = bottomRight;
            }
        }
        
        unsigned int ebo;
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &ebo);

        // TODO(CK):
        // Having all of these buffers is probably bad
        // I am sure you could easily just use 1 array and fill it up with all of this data
        // or just use a vector with the Vert structure for the data that is the proper way...
        // REFACTOR

        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(FVertex), this->vertices, GL_STATIC_DRAW);

        // STUDY(CK): Why do we need to use indices??? what is the purpose of them...
        // lots of fundamentals missing 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(int), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)0);

        // Vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, normal));

        // Vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, texCoords));

        // TODO(CK) & STUDY(CK): I forgot 3 * sizeof(float) in the last parameter 
        // I need to make sure that I study glBufferData and glVertexAttribPointer I think because 
        // it was missing the stride it wasn't formating the data correctly which is why it looked
        // like a mountain
        //glEnableVertexAttribArray(0);
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


        // STUDY(CK): For some reason at the very end of this (void*)(6 * sizeof(float))); 
        // was causing the texcoords to distort.. this is probably why you are meant to have
        // ONE BUFFER bound instead 
       // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        // --------------------------

        // unbind & clean up
        glBindVertexArray(0);
        delete[] indices;
    }

    // TODO(CK): Move somewhere global
    #define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
    void Terrain::GenerateGrass()
    {
        // TODO(CK): This is the worst way to do this...
        // we need a grid system on top of the terrain 
        // the grid locations need to be decently sized

        int modelIndex = 0;
        // positions are in vertices
        for (int i = 0; i < (VERTEX_COUNT*VERTEX_COUNT); ++i)
        {
            // TEST
            if (i >= 0 && i < 10)
            {
                // this is gross but w/e
                CreateEmptyObject();
                LoadEmptyObject(modelIndex, "TradeMan.obj", "content/objects/treasureteller/TradeMan.obj");
                g_objects[modelIndex]->pos.x = this->vertices[i].position.x;
                g_objects[modelIndex]->pos.y = this->vertices[i].position.y;
                g_objects[modelIndex]->pos.z = this->vertices[i].position.z;
                ++modelIndex;
            }
            if (i >= 600 && i < 620)
            {
                // this is gross but w/e
                CreateEmptyObject();
                LoadEmptyObject(modelIndex, "TradeMan.obj", "content/objects/treasureteller/TradeMan.obj");
                g_objects[modelIndex]->pos.x = this->vertices[i].position.x;
                g_objects[modelIndex]->pos.y = this->vertices[i].position.y;
                g_objects[modelIndex]->pos.z = this->vertices[i].position.z;
                ++modelIndex;

            }
            if (i >= 3000 && i < 3010)
            {
                // this is gross but w/e
                CreateEmptyObject();
                LoadEmptyObject(modelIndex, "TradeMan.obj", "content/objects/treasureteller/TradeMan.obj");
                g_objects[modelIndex]->pos.x = this->vertices[i].position.x;
                g_objects[modelIndex]->pos.y = this->vertices[i].position.y;
                g_objects[modelIndex]->pos.z = this->vertices[i].position.z;
                ++modelIndex;
            }
            if (i >= 10000 && i < 10010)
            {
                // this is gross but w/e
                CreateEmptyObject();
                LoadEmptyObject(modelIndex, "TradeMan.obj", "content/objects/treasureteller/TradeMan.obj");
                g_objects[modelIndex]->pos.x = this->vertices[i].position.x;
                g_objects[modelIndex]->pos.y = this->vertices[i].position.y;
                g_objects[modelIndex]->pos.z = this->vertices[i].position.z;
                ++modelIndex;
            }
        }
    }

};