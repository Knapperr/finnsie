#include "game_object.h"

namespace finnsie {

	GameObject InitGameObject(std::string name, glm::vec2 pos, glm::vec2 size, 
							  glm::vec2 velocity, glm::vec3 color, bool solid, 
							  float rotation, Texture2D sprite)
	{
		GameObject obj = {};
		obj.Name = name;
		obj.Pos = pos;
		obj.Size = size;
		obj.Velocity = velocity;
		obj.Color = color;
		obj.Solid = solid;
		obj.Rotation = rotation;
		obj.Sprite = sprite;
		return obj;
	}

	void Draw(GameObject* obj, SpriteRenderer& renderer)
	{
		renderer.Draw2DGameObject(obj->Sprite, obj->Pos, obj->Size, obj->Color, obj->Rotation);
	}

}