#ifndef CAMERA_HG_
#define CAMERA_HG_

/*
	FROM:
	https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h
*/

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "game_object.h"

namespace finnsie {

	enum class Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};
    
	const float YAW = -10.0f;
	const float PITCH = 0.0f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;
    
	class Camera
	{
    public:

		// TODO(CK): Refactor lower case variable names
		// Camera Attributes
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		// Euler Angles
		float Yaw;
		float Pitch;
		// Camera options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;
		bool LockedY;

		// 3rd person attributes
		GameObject* target;
		bool following;
		float distanceFromTarget;
        
		Camera(glm::vec3 position = glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
			: Front(glm::vec3(0.0f, 0.0f, -1.0f))
			, MovementSpeed(30.5f)
			, MouseSensitivity(SENSITIVITY)
			, Zoom(ZOOM)
		{
			LockedY = false;
			Position = position;
			WorldUp = up;
			Yaw = yaw;
			Pitch = pitch;
			following = false;
			updateCameraVectors();
		}

		
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
			: Front(glm::vec3(0.0f, 0.0f, -1.0f))
			, MovementSpeed(30.5f)
			, MouseSensitivity(SENSITIVITY)
			, Zoom(ZOOM)
		{
			LockedY = false;
			Position = glm::vec3(posX, posY, posZ);
			WorldUp = glm::vec3(upX, upY, upZ);
			Yaw = yaw;
			Pitch = pitch;
			following = false;
			updateCameraVectors();
		}
        
		Camera(GameObject obj)
		{
			target = &obj;
			LockedY = false;

			following = true;
			updateCameraVectors();
		}

		// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 GetViewMatrix();
		glm::mat4 ViewMatrix();
		// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void ProcessKeyboard(Camera_Movement direction, float deltaTime);
		// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
		// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void ProcessMouseScroll(float yoffset);

        
    private:
		// Calculates the front vector from the Camera's (updated) Euler Angles
		void updateCameraVectors();
	};
}
#endif