#ifndef TERRAIN_H
#define TERRAIN_H

#include <glm/glm.hpp>

#include "game_object.h"
#include "camera.h"

namespace finnsie {
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

    // TODO(CK): 
    // Tile Chunks
    // Terrain needs a grid for pathfinding... and other things as well like placing items 
    // collision detection? stuff like that
    struct Grass
    {
        glm::mat4* matrices;
        Model model;
        int amount = 10000;
        int textureId;
    };

    // TODO(CK): Just like handmade we need a coord system for the terain
    class Terrain
    {
    public:
        unsigned int VBO;
        unsigned int VAO;
        unsigned int indicesLength;
        int textureId;
        float x;
        float z;
        bool wireFrame;

        // TODO(CK): Bad practice?
        // if default constructor gets called this breaks because its being
        // initialized twice
        // I don't think I need this.. i guess if the vertices change?
        FVertex* vertices;
        Grass grass;

        Terrain(int gridX, int gridZ);
        ~Terrain();

        // Method from java video
        void Generate();
        void GenerateGrass();

    };

}
#endif