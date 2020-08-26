#ifndef CUBE_MAP_HG_
#define CUBE_MAP_HG_

#include "renderer.h"

namespace finnsie {

    struct Cubemap
    {
        // delete these buffers
        unsigned int VAO;
        unsigned int VBO;
        unsigned int textureID;
    };

    unsigned int LoadCubemapTextures(std::vector<std::string> faces);
    void SetupCubemap(Cubemap* cubemap);
    void DrawCubemap(Cubemap* cubemap, Shader* shader, Camera* cam);
    void DestroyCubemap(Cubemap* cubemap);
}

#endif