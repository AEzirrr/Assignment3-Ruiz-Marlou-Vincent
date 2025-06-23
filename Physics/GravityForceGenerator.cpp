#include "GravityForceGenerator.h"

namespace Physics {
	void GravityForceGenerator::UpdateForce(P6Particle* particle, float time) {

		if (particle->mass <= 0.0f) return; // Prevent division by zero
		MyVector force = Gravity * particle->mass;
		particle->AddForce(force);
	}
}