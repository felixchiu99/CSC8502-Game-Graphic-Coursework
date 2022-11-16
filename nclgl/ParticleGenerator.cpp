#include "ParticleGenerator.h"
/*
ParticleGenerator::ParticleGenerator() {
	// create 100 particles
	this->particles.resize(100);
	for (int i = 0; i < this->particles.size(); ++i)
	{
		// give every particle a random position
		float random = ((rand() % 100) - 50) / 10.0f;
		this->particles[i].position = GetWorldTransform().GetPositionVector() * random;
		this->particles[i].lifetime = ((rand() % 100)+100) / 10.0f;
	}

	// create a vertex and position buffer
	this->vertexBuffer = glGenBuffer();
	this->positionBuffer = lglGenBuffer();

	// fill the vertex buffer
	std::vector< float > vertices;
	vertices.push_back(0.0f);
	vertices.push_back(0.0f);
	vertices.push_back(0.0f);

	vertices.push_back(1.0f);
	vertices.push_back(0.0f);
	vertices.push_back(0.0f);

	vertices.push_back(0.0f);
	vertices.push_back(1.0f);
	vertices.push_back(0.0f);

	vertices.push_back(1.0f);
	vertices.push_back(1.0f);
	vertices.push_back(0.0f);

	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// fill the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->particles.size() * 4 * sizeof(float), this->positions, GL_DYNAMIC_DRAW);
}
ParticleGenerator::~ParticleGenerator()
{
	glDeleteBuffer(this->vertexBuffer);
	glDeleteBuffer(this->positionBuffer);
}

void ParticleGenerator::Update(const float dt) {
	for (int i = 0; i < this->particles.size(); ++i)
	{
		// subtract from the particles lifetime
		this->particles[i].lifetime -= dt;

		// if the lifetime is below 0 respawn the particle
		if (this->particles[i].lifetime <= 0.0f)
		{
			this->particles[i].position = lstd_random_between(Vector3(-1.0f), vec3(1.0f));
			this->particles[i].lifetime = lstd_random_between(1.0f, 2.0f);
		}

		// move the particle down depending on the delta time
		this->particles[i].position -= Vector3(0.0f, dt * 2.0f, 0.0f);

		// update the position buffer
		this->positions[i * 4 + 0] = this->particles[i].position[0];
		this->positions[i * 4 + 1] = this->particles[i].position[1];
		this->positions[i * 4 + 2] = this->particles[i].position[2];
		this->positions[i * 4 + 3] = this->particles[i].lifetime;
	}
}
void ParticleGenerator :: Draw(void) {

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(4);

	// update the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->particles.size() * 4 * sizeof(float), this->positions);

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// position buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribDivisor(4, 1);

	// draw triangles
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, this->particles.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(4);
}
*/