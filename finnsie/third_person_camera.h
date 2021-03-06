#ifndef THIRD_PERSON_CAMERA_HG_
#define THIRD_PERSON_CAMERA_HG_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game_object.h"

namespace finnsie {
	class ThirdPersonCamera {
	public:
		glm::vec3 position;
		float yaw;
		float pitch;
		float roll;

		float distanceFromTarget = 50;
		float angleAroundTarget = 0;

		GameObject* target;

		ThirdPersonCamera(GameObject* obj);

		// Add to math file
		// use this to Slerp the camera
		// back to the player
		float Slerp();

		void Move();
		void CalculateZoom(float yoffset);
		void ProcessMouse(float xoffset, float yoffset);
		glm::mat4 ViewMatrix();
	private:
		float const CONSTRAINT_PITCH_VAL = -1.10f;

		void calculateAngleAroundPlayer(float xoffset);
		void calculateCameraPosition();
		void calculatePitch(float yoffset);


	};
}

#endif
