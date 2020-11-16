#include "third_person_camera.h"


#include "game.h"

#include <glm/mat4x4.hpp> 
#include <glm/vec3.hpp>
#include <glm/vec4.hpp> 
#include <glm/gtc/quaternion.hpp>

namespace finnsie {
	ThirdPersonCamera::ThirdPersonCamera(GameObject* obj)
	{
		position = glm::vec3(1.0f);
		target = obj;
		pitch = 20;
		yaw = 0;
		roll = 0;
	}

	glm::mat4 ThirdPersonCamera::ViewMatrix()
	{
		glm::mat4 viewMatrix = glm::mat4(1.0f);
		viewMatrix = glm::rotate(viewMatrix, glm::radians(pitch), glm::vec3(1, 0, 0));
		viewMatrix = glm::rotate(viewMatrix, glm::radians(yaw), glm::vec3(0, 1, 0));
		glm::vec3 cameraPos = position;
		glm::vec3 negativeCameraPos = glm::vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z);
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), negativeCameraPos);
		viewMatrix = viewMatrix * translate;

		return viewMatrix;
	}

	void ThirdPersonCamera::Move()
	{
		// these are being done every frame.....
		// calczoom 
		// calcpitch
		// calculateanglearoundplayer
		// ---------------
		float offsety = 2.0f;

		float horizontalDistance = (float)(distanceFromTarget * cosf(glm::radians(pitch)));
		float verticalDistance = (float)(distanceFromTarget * sinf(glm::radians(pitch)));
		float theta = glm::radians(target->orientation.y) + angleAroundTarget;
		float offsetx = horizontalDistance * sinf(glm::radians(theta));
		float offsetz = horizontalDistance * cosf(glm::radians(theta));
		position.x = target->pos.x - offsetx;
		position.z = target->pos.z - offsetz;
		position.y = (target->pos.y + verticalDistance) + offsety; // need some kind of offset for the target so the camera doesn't point at the floot
		yaw = 180 - (glm::radians(target->orientation.y) + angleAroundTarget);

		/*
		if the left button is not being pressed slerp back behind the
		player camera should always be in some deadzone behind the player

		*/
		// TODO(ck): SLERP the camera back 
		// before 
		//if (!g_Game->leftMousePressed)
		//{
		//	glm::quat cameraQuat = glm::quat(position.x, position.y, position.z, 0.0f);
		//	glm::quat targetQuat = glm::quat(target->pos.x, target->pos.y, target->pos.z, 0.0f);
		//	glm::quat newQuat = glm::slerp(cameraQuat, targetQuat, 0.3f);

		//	const glm::mat4 inverted = glm::inverse(ViewMatrix());
		//	glm::vec3 forward = glm::normalize(glm::vec3(inverted[2]));
		//	glm::vec3 newFwd = newQuat * forward;

		//	position = newFwd;
		//}
		
	}
	

	void ThirdPersonCamera::ProcessMouse(float xoffset, float yoffset)
	{
		calculatePitch(yoffset);
		calculateAngleAroundPlayer(xoffset);
	}

	void ThirdPersonCamera::CalculateZoom(float yoffset)
	{
		float zoom = yoffset * 0.3f;
		distanceFromTarget -= zoom;
	}

	void ThirdPersonCamera::calculatePitch(float yoffset)
	{
		if (g_Game->leftMousePressed)
		{
			float newPitch = yoffset * 0.1f; // sensitivity
			this->pitch -= newPitch;
			
			// Constrain pitch from going under terrain
			if (this->pitch < CONSTRAINT_PITCH_VAL)
				this->pitch = CONSTRAINT_PITCH_VAL;
		}
	}

	void ThirdPersonCamera::calculateAngleAroundPlayer(float xoffset)
	{
		if (g_Game->leftMousePressed)
		{
			float angle = xoffset * 0.3f; // sensitivity
			this->angleAroundTarget -= angle;
		}
	}


	// REMOVE
	void ThirdPersonCamera::calculateCameraPosition()
	{
		return;
	}
}