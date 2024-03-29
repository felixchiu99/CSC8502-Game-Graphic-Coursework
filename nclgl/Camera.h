#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include <algorithm>
class Camera {
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
		row = 0.0f;
		moveSpeed = 1000.0f;
		usingFreeLook = false;
	};
	Camera(float pitch, float yaw, Vector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
		row = 0.0f;
		moveSpeed = 1000.0f;
		usingFreeLook = false;
	}
	~Camera(void) {};
	void UpdateCamera(float dt = 1.0f);
	
	Matrix4 BuildViewMatrix();
	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }
	float GetYaw() const { return yaw; }
	void SetYaw(float y) { 
		yaw = y;
		if (yaw < 0) {
			yaw += 360.0f;

		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}
	}
	float GetPitch() const { return pitch; }
	void SetPitch(float p) { 
		pitch = p;
		pitch = std::min(pitch, 90.0f);
		pitch = std::max(pitch, -90.0f);
	}
	float GetRow() const { return row; }
	void SetRow(float r) { row = r; }

	float GetMoveSpeed() const { return moveSpeed; }

	void setFreeLook(bool free) { usingFreeLook = free; }

protected:
	float yaw;
	float pitch;
	float row;
	float moveSpeed;
	bool usingFreeLook;
	Vector3 position; //Set to 0,0,0 by Vector3 constructor ;)
};