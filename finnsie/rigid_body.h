#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <glm/gtc/matrix_transform.hpp> 

namespace finnsie {
    
    struct RigidBody
    {
        glm::vec2 velocity;
        glm::vec2 force;
        glm::vec2 acc;
        
        float mass;
    };
    
    void SetupRigidBody(RigidBody* rb, float gravity);
    void AddAcc(RigidBody *rb, float dt);
}
#endif //RIGID_BODY_H
