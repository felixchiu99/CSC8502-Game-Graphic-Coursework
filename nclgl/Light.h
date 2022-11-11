#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light() {} // Default constructor , we’ll be needing this later!
	Light(const Vector3 & position, const Vector4 & colour, float radius, int type = 0) {
		this->type = type;
		this->position = position;
		this->colour = colour;
		this->radius = radius;
		this->intensity = 50000;
	}

	~Light(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3 & val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4 & val) { colour = val; }

	float GetIntensity() const { return intensity; }
	void SetIntensity(float val) { intensity = val; }

	int GetType() const { return type; }

	virtual Vector3 GetFacingPoint() const { return Vector3(0, 0, 0); }
	virtual void SetFacingPoint(const Vector3& val) {}

	virtual Vector3 GetDirection() const { return Vector3(0, 0, 0); }

	virtual float GetAngle() const { return 45.0f; }

protected:
	Vector3 position;
	float radius;
	Vector4 colour;
	float intensity;
	int type; // 0 == point, 1 == direction, 2 == spot
};