#include "P6Particle.h"

namespace Physics {
	P6Particle::P6Particle() {
		this->position = MyVector();
		this->velocity = MyVector();
		this->acceleration = MyVector();
		this->mass = 0.0f; // Default mass
	}

	void P6Particle::UpdatePosition(float time) {

		this->position = this->position + (this->velocity * time) + ((this->acceleration * time * time) * (1.0f / 2.0f));
	}

	void P6Particle::UpdateVelocity(float time) {

		this->acceleration += accumulatedForce * (1.0f / mass); // Update acceleration based on accumulated force

		this->velocity = this->velocity + (this->acceleration * time);
		this->velocity = this->velocity * powf(damping, time); // Apply damping
	}

	void P6Particle::Update(float time) {
		UpdatePosition(time);
		UpdateVelocity(time);

		this->ResetForce();
	}

	void P6Particle::AddForce(MyVector force) {
		this->accumulatedForce += force;
	}

	void P6Particle::ResetForce() {
		this->accumulatedForce = MyVector(0, 0, 0);
		this->acceleration = MyVector(0, 0, 0);
	}

	bool P6Particle::IsDestroyed() {
		return isDestroyed;
	}

	void P6Particle::Destroy() {
		isDestroyed = true;
	}
}