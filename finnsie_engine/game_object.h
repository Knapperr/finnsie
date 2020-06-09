#ifndef GAME_OBJECT_HG_
#define GAME_OBJECT_HG_

#include <glm/gtc/matrix_transform.hpp> 
#include <string>

#include "model.h"

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

		GameObject()
		{
			this->pos = glm::vec3(1.0);
			this->orientation = glm::vec3(1.0);
			this->name = "Empty";
			this->scale = 1.0;
			this->viewNormals = false;
			this->wireFrame = false;

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

			// Load the model
			model = new Model(path);
		}

		~GameObject()
		{
			delete model;
		}
	};

}
#endif