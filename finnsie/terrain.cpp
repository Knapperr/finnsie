#include "terrain.h"

#include "utils.h"
#include <string>
#include <glm/gtc/type_ptr.hpp>

#define SIZE 800
#define VERTEX_COUNT 128

namespace finnsie {

    Terrain::Terrain()
    {
        this->x = 0;
        this->z = 0;

        this->vao = 0;
        this->indicesLength = 0;
        this->textureId = 0;
    }

    Terrain::Terrain(int gridX, int gridZ)
    {
        this->x = gridX * SIZE;
        this->z = gridZ * SIZE;

        this->vao = 0;
        this->indicesLength = 0;
        this->textureId = 0;


        // Load a texture 
        std::string textPath = "content/textures/terr/grass.jpg";
        std::string textDir = textPath.substr(0, textPath.find_last_of('/'));
        textureId = LoadTextureFile("grass.jpg", textDir, false);
    }

    // TODO(CK):
    // Look at Brackeys: https://www.youtube.com/watch?v=64NblGkAabk&t=4s
    // this is probably a more sane way to generate and it will
    // actually teach you
    // USE THE VERT from the model move to global change name to *FVertex* finnsie vertex
    struct FVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    void Terrain::Generate()
    {
        // TODO(CK): IMPORTANT
        /*
            Completely refactor this to use the Vert struct
            Should be doing this the same way as the mesh/model using
            the struct for the stride
            I think I can use arrays still because the mesh & model uses
            vectors I can just use one array for vertices/normals/textureCoords
            The only issue is the size of it but it should be
            count(256) * (3+3+2) = 2048
            I don't think that size is that bad... the vectors for
            the models such as the crysis model are 15222 so yes that is nothing....
            that is more than triple the size of our tiny terrain... this is good news

        */
        const int count = VERTEX_COUNT * VERTEX_COUNT;
        FVertex* vertices = new FVertex[count];
        int* indices = new int[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1)];

        int verticesLength = count;
        this->indicesLength = 6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1);

        int index = 0;
        for (int i = 0; i < VERTEX_COUNT; ++i)
        {
            for (int j = 0; j < VERTEX_COUNT; ++j)
            {
                // Vertices
                vertices[index].position.x = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
                vertices[index].position.y = 0;
                vertices[index].position.z = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;

                // Normals
                vertices[index].normal.x = 0;
                vertices[index].normal.y = 1;
                vertices[index].normal.z = 0;

                // Texture Coords
                vertices[index].texCoords.x = (float)j / ((float)VERTEX_COUNT - 1);
                vertices[index].texCoords.y = (float)i / ((float)VERTEX_COUNT - 1);
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
        unsigned int vbo;
        //unsigned int nbo; // normal buffer
        //unsigned int tbo; // texture buffer
        unsigned int ebo;

        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        //glGenBuffers(1, &nbo);
        //glGenBuffers(1, &tbo);

        // TODO(CK):
        // Having all of these buffers is probably bad
        // I am sure you could easily just use 1 array and fill it up with all of this data
        // or just use a vector with the Vert structure for the data that is the proper way...
        // REFACTOR

        glBindVertexArray(this->vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(FVertex), vertices, GL_STATIC_DRAW);

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


        // THIS IS ALL WRONG ... get this working properly
        // need to figure out the strides...
        // ---------------------------------------------------
        // Vertex normals
        //glBindBuffer(GL_ARRAY_BUFFER, nbo);
        //glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(float), normals, GL_STATIC_DRAW);

        //glEnableVertexAttribArray(1);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(3 * sizeof(float)));
        // -------------------------

        // Vertex texture coords
        //glBindBuffer(GL_ARRAY_BUFFER, tbo);
        //glBufferData(GL_ARRAY_BUFFER, textureLength * sizeof(float), textureCoords, GL_STATIC_DRAW);

        //glEnableVertexAttribArray(2);
        // STUDY(CK): For some reason at the very end of this (void*)(6 * sizeof(float))); 
        // was causing the texcoords to distort.. this is probably why you are meant to have
        // ONE BUFFER bound instead 
       // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        // --------------------------

        // unbind & clean up
        glBindVertexArray(0);

        delete[] vertices;
        //delete[] normals;
       // delete[] textureCoords;
        delete[] indices;
    }

    //void Terrain::Render(Shader* modelShader, Camera* cam)
    //{
    //    glUseProgram(modelShader->id);
    //    glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)1080 / (float)720, 1.0f, 1000.0f);
    //    glUniformMatrix4fv(GetLoc(modelShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    //    glUniformMatrix4fv(GetLoc(modelShader, "view"), 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));

    //    glUniform3fv(GetLoc(modelShader, "lightPos"), 1, &g_lamp[0]);
    //    glUniform3fv(GetLoc(modelShader, "viewPos"), 1, &cam->Position[0]);

    //    // now set the sampler to the correct texture unit
    //    glUniform1i(glGetUniformLocation(modelShader->id, "texture_diffuse1"), 0);
    //    // and finally bind the texture		
    //    glBindTexture(GL_TEXTURE_2D, this->textureId);

    //    glm::mat4 matModel = glm::mat4(1.0f);

    //    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
    //                                            glm::vec3(this->x - 100.0f, -35.0f, this->z - 30.0));
    //    matModel = matModel * matTranslate;

    //    glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
    //                                    0.0f,
    //                                    glm::vec3(0.0f, 0.0f, 1.0f));
    //    matModel = matModel * rotateZ;

    //    glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
    //                                    0.0f,
    //                                    glm::vec3(0.0f, 1.0f, 0.0f));
    //    matModel = matModel * rotateY;

    //    glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
    //                                    0.0f,
    //                                    glm::vec3(1.0f, 0.0f, 0.0f));
    //    matModel = matModel * rotateX;

    //    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
    //                                    glm::vec3(0.1, 0.1, 0.1));

    //    matModel = matModel * matScale;
    //    glUniformMatrix4fv(GetLoc(modelShader, "model"), 1, GL_FALSE, glm::value_ptr(matModel));

    //    // Wireframe
    //    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //    glBindVertexArray(this->vao);
    //    glDrawElements(GL_TRIANGLES, this->indicesLength, GL_UNSIGNED_INT, 0);
    //    glBindVertexArray(0);

    //    // Always good practice to set everything back to defaults once configured
    //    // NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
    //    glActiveTexture(GL_TEXTURE0);
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //}
};