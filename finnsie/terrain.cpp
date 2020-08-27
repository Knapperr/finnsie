#include "terrain.h"

#include <string>
#include <glm/gtc/type_ptr.hpp>

#define SIZE 400
#define VERTEX_COUNT 128

#include "utils.h"
#include "gl_common.h"

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

#include <time.h>
    void Terrain::Generate()
    {
        int* indices = new int[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1)];

        int verticesLength = VERTEX_COUNT * VERTEX_COUNT;
        this->indicesLength = 6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1);

        int index = 0;
        srand(time(NULL));
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
        this->grass = {};
        this->grass.amount = 10000;
        this->grass.model = Model("content/objects/Grass/sht_grass.obj");
        grass.matrices = new glm::mat4[this->grass.amount];



        srand(time(NULL));

        srand(glfwGetTime()); // initialize random seed	
        float radius = 150.0;
        float offset = 25.0f;
        for (unsigned int i = 0; i < this->grass.amount; i += 3)
        {
            glm::mat4 model = glm::mat4(1.0f);
            // 1. translation: displace along circle with 'radius' in range [-offset, offset]
            float angle = (float)i / (float)this->grass.amount * 360.0f;
            float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float x = sin(angle) * radius + displacement;
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float z = cos(angle) * radius + displacement;
            model = glm::translate(model, glm::vec3(x, y, z));

            // 2. scale: Scale between 0.05 and 0.25f
            float scale = (rand() % 20) / 100.0f + 0.05;
            model = glm::scale(model, glm::vec3(scale));

            // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
            float rotAngle = (rand() % 360);
            model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
            
            // 4. now add to list of matrices
            this->grass.matrices[i] = model;

            model = glm::rotate(model, rotAngle + 180, glm::vec3(0.4f, 0.6f, 0.8f));
            this->grass.matrices[i + 1] = model;

            model = glm::rotate(model, rotAngle + 180, glm::vec3(0.4f, 0.6f, 0.8f));
            this->grass.matrices[i + 2] = model;
        }

        // configure instanced array
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, this->grass.amount * sizeof(glm::mat4), &this->grass.matrices[0], GL_STATIC_DRAW);

        for (unsigned int i = 0; i < this->grass.model.meshes.size(); i++)
        {
        }

        // TODO(CK): This is the worst way to do this...
        // we need a grid system on top of the terrain 
        // the grid locations need to be decently sized
        //int modelIndex = 0;
        //// positions are in vertices
        //for (int i = 0; i < (VERTEX_COUNT*VERTEX_COUNT); ++i)
        //{
        //    // TEST
        //    if (i >= 0 && i < 30)
        //    {
        //        // Use a raw array of grass to render and instance it onto the gpu
        //        // also randomize positions
        //        CreateEmptyObject();
        //        CreateEmptyObject();
        //        CreateEmptyObject();
        //        LoadEmptyObject(modelIndex, "grass", "content/objects/Grass/sht_grass.obj");
        //        LoadEmptyObject(modelIndex+1, "grass", "content/objects/Grass/sht_grass.obj");
        //        LoadEmptyObject(modelIndex+2, "grass", "content/objects/Grass/sht_grass.obj");

        //        int x = rand() % 10 + this->vertices[i].position.x;
        //        int z = rand() % 5 + this->vertices[i].position.z;

        //        g_objects[modelIndex]->pos.x = (float)x;
        //        g_objects[modelIndex]->pos.y = 0;
        //        g_objects[modelIndex]->pos.z = (float)z;
        //        g_objects[modelIndex]->scale = 0.3f;
        //        g_objects[modelIndex+1]->pos.x = (float)x;
        //        g_objects[modelIndex+1]->pos.y = 0;
        //        g_objects[modelIndex+1]->pos.z = (float)z;
        //        g_objects[modelIndex+1]->scale = 0.3f;
        //        g_objects[modelIndex+2]->pos.x = (float)x;
        //        g_objects[modelIndex+2]->pos.y = 0;
        //        g_objects[modelIndex+2]->pos.z = (float)z;
        //        g_objects[modelIndex+2]->scale = 0.3f;
        //        g_objects[modelIndex+1]->orientation = glm::vec3(0.0f, 1.0f, 0.0f);
        //        g_objects[modelIndex+2]->orientation = glm::vec3(0.0f, 0.5f, 0.0f);

        //        modelIndex += 3;
        //    }
        //}
    }

};