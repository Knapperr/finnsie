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
        
        // TODO(CK):
        // not sure if force
        // or acceleration should be in here...
        // they are separate things 
    };
    
    void SetupRigidBody(RigidBody* rb, float gravity);
    void AddAcc(RigidBody *rb, float dt);
}
#endif //RIGID_BODY_H
