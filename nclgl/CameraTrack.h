#pragma once
#include "../nclgl/OGLRenderer.h"
class CameraTrack
{
public:
	CameraTrack(Vector3 position, float pitch, float yaw, float positionSpeed = 1, float viewSpeed = 0.01) {
		this->position = position;
		this->yaw = yaw;
		this->pitch = pitch;
		this->positionSpeed = positionSpeed;
		this->viewSpeed = viewSpeed;
	};
	Vector3 GetPosition() const { return position; }
	float GetYaw() const { return yaw; }
	float GetPitch() const { return pitch; }
	float GetPositionSpeed() const{ return positionSpeed; }
	float GetViewSpeed() const { return viewSpeed; }
protected:
	Vector3 position;
	float yaw;
	float pitch;
	float positionSpeed;
	float viewSpeed;
};

