#ifndef PLAYER_HG_
#define PLAYER_HG_

#include "game_object.h"

// NOTE(CK): Reference: https://www.youtube.com/watch?v=d-kuzyCkjoQ
//		     Thin Matrix
namespace finnsie {

	struct PlayerInput {
		bool forward;
		bool backward;
		bool left;
		bool right;
		bool up;
		bool down;
	};

	class Player : public GameObject {
	public:

		Player(std::string name, glm::vec3 pos, glm::vec3 orientation, float scale, std::string path);

		float speed = 28.0f;
		float turnSpeed = 180.0f;
		float currentSpeed = 0;
		float currentTurnSpeed = 0;
		glm::vec3 rotation;

		void Move(PlayerInput input, float dt);
		void CheckInputs(PlayerInput input);
		void IncreasePosition(float dx, float dy, float dz);
		void IncreaseRotation(float dx, float dy, float dz);
		void ResetInput(PlayerInput *input);
	};
}
#endif
