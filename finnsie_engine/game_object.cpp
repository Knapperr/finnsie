#include "game_object.h"

namespace finnsie {

	GameObject::GameObject(std::string name, glm::vec2 pos, glm::vec2 size, glm::vec2 velocity,
		glm::vec3 color, bool solid, float rotation, Texture sprite)
		: Name(name)
		, Pos(pos)
		, Size(size)
		, Velocity(velocity)
		, Color(color)
		, Solid(solid)
		, Rotation(rotation)
		, Sprite(sprite)
	{
	}

	void GameObject::Draw(SpriteRenderer& renderer)
	{
		renderer.DrawGameObject(this->Sprite, this->Pos, this->Size, this->Color, this->Rotation);
	}

}