#ifndef GAME_OBJECT_HG_
#define GAME_OBJECT_HG_

#include "model.h"
#include "rigid_body.h"

namespace finnsie {
    
	class GameObject
	{
        public:
		Model* model = {};
        
		glm::vec3 pos;
		glm::vec3 orientation;
		std::string name;
		float scale;
		bool viewNormals;
		bool wireFrame;
        
        RigidBody rb;
        
		GameObject()
		{
			this->pos = glm::vec3(1.0);
			this->orientation = glm::vec3(1.0);
			this->name = "Empty";
			this->scale = 1.0;
			this->viewNormals = false;
			this->wireFrame = false;
            this->rb = {};
            SetupRigidBody(&rb, -9.81f);
            
            // NOTE(CK): Model will have 0 meshes loaded
            // keep pointer pointing to nothing
            //model = new Model();
		}
        
		GameObject(std::string name, glm::vec3 pos, glm::vec3 orientation, float scale, std::string path)
		{
			this->name = name;
			this->pos = pos;
			this->orientation = orientation;
			this->scale = scale;
			this->viewNormals = false;
			this->wireFrame = false;
            
            this->rb = {};
            SetupRigidBody(&rb, -9.81f);
            
			// Load the model
			model = new Model(path);
		}
        
        
		~GameObject()
		{
            delete model;
            model = NULL;
		}
	};
    
    class WaterObject : public GameObject 
    {
        public:
        float uJump = 0.25f;
		float vJump = 0.25f;
		float tiling = 1.0f;
		float speed = 0.2f;
		float flowStrength = 0.07f;
		float flowOffset = -0.207f;
		float heightScale = 0.1f;
		float heightScaleModulated = 9.0f;
		float gridResolution = 10.0f;
		float tilingModulated = 50.0f;
		bool dualGrid = false;
        
        
        WaterObject(std::string name, glm::vec3 pos, 
                    glm::vec3 orientation, float scale, 
                    std::string path) 
            : GameObject(name, pos, orientation, scale, path)
		{
		}
        
    };
}
#endif