#ifndef TERRAIN_H
#define TERRAIN_H

#include "gl_common.h"
#include <glm/glm.hpp>

#include "camera.h"

namespace finnsie {
    class Terrain
    {
    public:
        float x;
        float z;

        int textureId;
        unsigned int vao;
        unsigned int indicesLength;

        Terrain();
        Terrain(int gridX, int gridZ);
        void Generate();

        /*
        remember to clean up
        delete arrays and buffers from terrain
        glDeleteVertexArrays(1, &this->cubeVAO);
        glDeleteVertexArrays(1, &this->lampVAO);
        glDeleteBuffers(1, &this->VBO);
        */
    };

}
#endif