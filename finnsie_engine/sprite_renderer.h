#ifndef SPRITE_RENDERER_HG_
#define SPRITE_RENDERER_HG_


#include "shader_manager.h"
#include "texture_manager.h"

namespace finnsie {

	class SpriteRenderer {
	public:
		SpriteRenderer(Shader& shader);
		~SpriteRenderer();
		void Draw2DGameObject(Texture2D sprite, glm::vec2 pos, glm::vec2 size, glm::vec3 color, float rotation);

	private:
		Shader shader;
		unsigned int quadVAO;
		void initRenderData();
	};
}

#endif