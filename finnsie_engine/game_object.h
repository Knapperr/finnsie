#ifndef GAME_OBJECT_HG_
#define GAME_OBJECT_HG_

#include "texture_manager.h"
#include "sprite_renderer.h"
#include <glm/vec3.hpp> // glm::vec3
#include <string>

namespace finnsie {

	struct GameObject {
		std::string Name;
		glm::vec2 Pos;
		glm::vec2 Size;
		glm::vec2 Velocity;
		glm::vec3 Color;
		bool Solid;
		float Rotation;
		bool Destroyed;
		Texture Sprite;
	};

	GameObject InitGameObject(std::string name, glm::vec2 pos, glm::vec2 size, 
							  glm::vec2 velocity, glm::vec3 color, bool solid, 
							  float rotation, Texture sprite);

	void Draw(GameObject* obj, SpriteRenderer& renderer);
}

#endif