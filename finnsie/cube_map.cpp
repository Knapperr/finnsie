#include "cube_map.h"

namespace finnsie {

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    // something more performant than a vector of strings? 
    // its 6 texture locations .. const char* faces[6]
    unsigned int LoadCubemapTextures(std::vector<std::string> faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); ++i)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                LOG("Cubemap failed to load");
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    void SetupCubemap(Cubemap* cubemap)
    {
        // Class for this or struct..?
        // probably just a struct with the 
        glGenVertexArrays(1, &cubemap->VAO);
        glGenBuffers(1, &cubemap->VBO);
        glBindVertexArray(cubemap->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubemap->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


        std::vector<std::string> faces
        {
            "content/textures/skyboxsun5deg2/right.bmp", // right
            "content/textures/skyboxsun5deg2/left.bmp", // left
            "content/textures/skyboxsun5deg2/top.bmp", // top
            "content/textures/skyboxsun5deg2/bottom.bmp",        // bottom
            "content/textures/skyboxsun5deg2/front.bmp", // front
            "content/textures/skyboxsun5deg2/back.bmp", // back
        };

        cubemap->textureID = LoadCubemapTextures(faces);
    }

    // TODO(CK): Move to renderer:
    // do not need the camera... we only use the GetViewMatrix
    // just pass that directly through
    void DrawCubemap(Cubemap* cubemap, Shader* shader, Camera* cam)
    {
        glStencilMask(0x00);
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        glUseProgram(shader->id);
        glm::mat4 view = glm::mat4(glm::mat3(cam->GetViewMatrix()));

        glUniformMatrix4fv(GetLoc(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom),
                                                (float)1080 / (float)720,
                                                1.0f, 1000.0f);

        glUniformMatrix4fv(GetLoc(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(cubemap->VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        //glDepthFunc(GL_LESS); // back to default
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // I think i want this as default though
    }

    void DestroyCubemap(Cubemap* cubemap)
    {
        glDeleteVertexArrays(1, &cubemap->VAO);
        glDeleteBuffers(1, &cubemap->VBO);
    }
}