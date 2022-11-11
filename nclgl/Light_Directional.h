#pragma once
#include "Light.h"
class Light_Directional : public Light
	{
	public:
		Light_Directional() {} // Default constructor , we’ll be needing this later!
		Light_Directional(const Vector3& position, const Vector4& colour, float radius, const Vector3 & facingPoint = Vector3(0, 0, 0)) {
			this->type = 1;
			this->position = position;
			this->facingPoint = facingPoint;
			this->colour = colour;
			this->radius = radius;
			this->intensity = 5000000;
		}

		~Light_Directional(void) {};

		Vector3 GetFacingPoint() const { return facingPoint; }
		void SetFacingPoint(const Vector3& val) { facingPoint = val; }

		Vector3 GetDirection() const { return position - facingPoint; }
	protected:
		Vector3 facingPoint;
		//float intensity;
	}; 

