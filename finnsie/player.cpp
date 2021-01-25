#include "player.h"

namespace finnsie {

	Player::Player(std::string name, glm::vec3 pos, glm::vec3 orientation, float scale, std::string path) 
		: GameObject(name, pos, orientation, scale, path)
	{
		rotation = glm::vec3(1.0);
	}

	void Player::Move(PlayerInput input, float dt)
	{
		CheckInputs(input);
		IncreaseRotation(0.0f, currentTurnSpeed * dt, 0.0f);
		float distance = currentSpeed * dt;
		float dx = distance * sinf(glm::radians(orientation.y));
		float dz = distance * cosf(glm::radians(orientation.y));
		IncreasePosition(dx, 0.0f, dz);
	}

	void Player::CheckInputs(PlayerInput input)
	{
		if (input.forward)
		{
			this->currentSpeed = speed;
		}
		else if (input.backward)
		{
			this->currentSpeed = -speed;
		}
		else
		{
			this->currentSpeed = 0.0f;
		}

		if (input.right)
		{
			this->currentTurnSpeed = -turnSpeed;
		}
		else if (input.left)
		{
			this->currentTurnSpeed = turnSpeed;
		}
		else
		{
			this->currentTurnSpeed = 0.0f;
		}

	}

	void Player::IncreasePosition(float dx, float dy, float dz)
	{
		this->pos.x += dx;
		this->pos.y += dy;
		this->pos.z += dz;
	}
	
	void Player::IncreaseRotation(float dx, float dy, float dz)
	{
		this->orientation.x += dx;
		this->orientation.y += dy;
		this->orientation.z += dz;
	}

	void Player::ResetInput(PlayerInput* input)
	{
		input->forward = false;
		input->backward = false;
		input->down = false;
		input->left = false;
		input->right = false;
		input->up = false;
	}
}