#ifndef TERRAIN_H
#define TERRAIN_H

#include <glm/glm.hpp>

#include "game_object.h"
#include "camera.h"

namespace finnsie {
    // TODO(CK): 
        // Tile Chunks
        // Terrain needs a grid for pathfinding... and other things as well like placing items 
        // collision detection? stuff like that
        
        // Just like handmade we need a coord system for the terain

    // TODO(CK): move to finnsie_types file or something
    struct FVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    struct Grass
    {
        glm::mat4* matrices;
        Model model;
        int amount = 10000;
        int textureId;
    };

    class Terrain
    {
    public:
        float x;
        float z;
        unsigned int VBO;
        unsigned int VAO;
        unsigned int indicesLength;
        
        const int textureIdCount = 4;
        int* textureIds;
        int selectedTextureId;
        bool wireFrame;
        bool drawTexture;

        // TODO(CK): Bad practice?
        // if default constructor gets called this breaks because its being
        // initialized twice
        // I don't think I need this.. i guess if the vertices change?
        FVertex* vertices;
        Grass grass;
        Shader* shader;
        Shader* grassShader;
        
        // TODO(CK): Run on a separate thread
        Terrain(int gridX, int gridZ);
        ~Terrain();

        void Generate();
        void GenerateGrass();
    };

}
#endif