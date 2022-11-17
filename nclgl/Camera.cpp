#include "Camera.h"
#include "Window.h"
#include <algorithm>
void Camera::UpdateCamera(float dt) {
	if (usingFreeLook) {
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);
	}
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
		
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	if (usingFreeLook) {
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_R)) {
			moveSpeed += dt * 100;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F)) {
			moveSpeed -= moveSpeed >0 ? dt * 1000:0;
		}
	}
	

	float speed = moveSpeed * dt;
	if (usingFreeLook) {
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			position += forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			position -= forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			position -= right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			position += right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			position.y += speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			position.y -= speed;
		}
	}
	/*
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q)) {
		row -= 1;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_E)) {
		row += 1;
	}
	*/
}
Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-row, Vector3(0, 0, 1)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
};