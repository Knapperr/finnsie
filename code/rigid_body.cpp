#include "rigid_body.h"

namespace finnsie {
    
    void SetupRigidBody(RigidBody* rb, float gravity)
    {
        rb->mass = 1.0f;
        rb->velocity = glm::vec2(1.0);
        // gravity -9.81f 
        rb->force = glm::vec2(0, rb->mass * gravity);
        rb->acc = glm::vec2(rb->force.x / rb->mass, rb->force.y / rb->mass);
    }
    
    void AddAcc(RigidBody *rb, float dt)
    {
        rb->velocity.x += rb->acc.x * dt;
        rb->velocity.y += rb->acc.y * dt;
    }
}