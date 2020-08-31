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
        delete[] grass.matrices;
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

        if (this->grass.matrices == NULL)
        {
            this->grass = {};
            this->grass.model = Model("content/objects/grass_star/Grass.obj");
            // TODO(CK): Need a getter for texture id
            this->grass.textureId = this->grass.model.textures_loaded[0].id;
            grass.matrices = new glm::mat4[this->grass.amount];
        }
        else
        {
            delete[] grass.matrices;
            grass.matrices = new glm::mat4[this->grass.amount];
        }

        srand(time(NULL));
        // TODO(CK): Shouldn't increment by 3 this is buggy
        for (unsigned int i = 0; i < this->grass.amount; i+=3)
        {
            glm::mat4 model = glm::mat4(1.0f);
            // 1. translation: displace along circle with 'radius' in range [-offset, offset]
            float x = rand() % 380 + 10;
            float y = 0;
            float z = rand() % 380 + 10;
            model = glm::translate(model, glm::vec3(x, y, z));

            // 2. scale: Scale between 0.05 and 0.25f
            float scale = 1.0f;
            model = glm::scale(model, glm::vec3(scale));
            
            // 4. now add to list of matrices
            this->grass.matrices[i] = model;

            if (i < (this->grass.amount - 3))
            {
                float rotAngle = 40.0f;
                model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
                assert((i + 1) < this->grass.amount);
                this->grass.matrices[i + 1] = model;

                rotAngle = 135.0f;
                model = glm::rotate(model, rotAngle, glm::vec3(0.0f, -1.0f, 0.0f));
                assert((i + 2) < this->grass.amount);
                this->grass.matrices[i + 2] = model;
            }
        }

        // configure instanced array
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, this->grass.amount * sizeof(glm::mat4), &this->grass.matrices[0], GL_STATIC_DRAW);

        for (unsigned int i = 0; i < this->grass.model.meshes.size(); i++)
        {
            unsigned int VAO = grass.model.meshes[i].VAO;
            glBindVertexArray(VAO);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);

            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));

            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));

            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }
    }
};