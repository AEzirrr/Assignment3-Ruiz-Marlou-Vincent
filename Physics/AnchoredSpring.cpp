#include "AnchoredSpring.h"

namespace Physics {

	void AnchoredSpring::UpdateForce(P6Particle* particle, float time) {
		MyVector pos = particle->position; // Get the position vector from the anchor to the particle

		MyVector force = pos - anchorPoint; // Calculate the vector from the anchor to the particle

		float mag = force.Magnitude(); // Get the magnitude of the force vector

		float springForce = -springConstant * abs(mag - restLength); // Calculate the spring force using Hooke's law

		force = force.Normalize(); // Normalize the force vector

		force = force * springForce; // Scale the force vector by the spring force

		particle->AddForce(force); // Apply the force to the particle	
	}
}