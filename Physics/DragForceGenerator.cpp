#include "DragForceGenerator.h"

namespace Physics{

	void DragForceGenerator::UpdateForce(P6Particle* particle, float time) {

		MyVector force = MyVector(0, 0, 0);
		MyVector currVelocity = particle->velocity;

		float mag = currVelocity.Magnitude();

		if (mag <= 0.0f) return; // Prevent division by zero

		float dragF = (k1 * mag) + (k2 * mag);
		MyVector dir = currVelocity.Normalize();

		particle->AddForce(dir * -dragF); // Apply drag force in the opposite direction of velocity
	}
}