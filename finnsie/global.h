#ifndef GLOBAL_HG_
#define GLOBAL_HG_

#include <glm/glm.hpp>	
#include <vector>


#include "log.h"
#include "terrain.h"

namespace finnsie {
    extern glm::vec3 g_lamp;
    
#define PI32 3.14159265359f
#define local_persist static 
#define global_variable static
    
    typedef int8_t int8;
    typedef int16_t int16;
    typedef int32_t int32;
    typedef int64_t int64;
    typedef int32 bool32;
    
    typedef uint8_t uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint64_t uint64;
    
    typedef float real32;
    typedef double real64;
    
    struct Fin_Vertex
    {
        float x, y;
        float r, g, b;
    };
    
}
#endif