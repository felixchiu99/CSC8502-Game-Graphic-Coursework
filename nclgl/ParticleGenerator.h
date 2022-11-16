#pragma once

#include "..\nclgl\SceneNode.h"

struct Particle {
	Vector2 Position, Velocity;
	Vector4 Color;
	float	Life;

	Particle()
		: Position(Vector2(0,0)), Velocity(Vector2(0, 0)), Color(Vector4(1, 1, 1, 1)), Life(0.0f) { }
};
class ParticleGenerator : public SceneNode
{
public:
	ParticleGenerator() {
	};
	void Update(const float dt);
	void Draw();
	

	
	
private:
	struct Particle
	{
		Vector3 position;
		float lifetime;
	};
	std::vector< Particle > particles;

	float positions[400];

	GLuint vertexBuffer;
	GLuint positionBuffer;
};

