#pragma once
#include "Light.h"
class Light_Spot :
	public Light
{
	public:
		Light_Spot() {} // Default constructor , we’ll be needing this later!
		Light_Spot(const Vector3& position, const Vector4& colour, float radius, const Vector3& facingPoint = Vector3(0, 0, 0), float angle = 45.0f) {
			this->type = 2;
			this->position = position;
			this->facingPoint = facingPoint;
			this->colour = colour;
			this->radius = radius;
			this->angle = angle;
			this->intensity = 50000;
		}

		~Light_Spot(void) {};

		Vector3 GetFacingPoint() const { return facingPoint; }
		void SetFacingPoint(const Vector3& val) { facingPoint = val; }

		Vector3 GetDirection() const { return position - facingPoint; }

		float GetAngle() const { return angle; }
		void SetAngle(float val) { angle = val; }
	protected:
		Vector3 facingPoint;
		float angle;

};

