#include "ParticleContact.h"
#include "MyVector.h"

namespace Physics {

	float ParticleContact::GetSeparatingSpeed() {

		MyVector velocity = particles[0]->velocity;

		if (particles[1]) velocity -= particles[1]->velocity;

		return velocity.ScalarProduct(contactNormal);
	}

	void ParticleContact::ResolveVelocity(float time) {

		float  separatingSpeed = GetSeparatingSpeed();

		if (separatingSpeed > 0.0f) {
			return;
		}

		float newSS = -restitution * separatingSpeed;

		float deltaSpeed = newSS - separatingSpeed;

		float totalMass = (float)1 / particles[0]->mass;

		if (particles[1]) totalMass += (float)1 / particles[1]->mass;

		if (totalMass <= 0.0f) return;

		float impulse_mag = deltaSpeed / totalMass;

		MyVector Impulse = contactNormal * impulse_mag;

		MyVector V_a = Impulse * ((float)1 / particles[0]->mass);

		particles[0]->velocity = particles[0]->velocity + V_a;

		if (particles[1]) {
			MyVector V_b = Impulse * ((float)-1 / particles[1]->mass);
			particles[1]->velocity = particles[1]->velocity + V_b;
		}

	}

	void ParticleContact::Resolve(float time) {
		ResolveVelocity(time);

		ResolveInterpenetration(time);

	}

	void ParticleContact::ResolveInterpenetration(float time) {

		if (depth <= 0.0f) return; // No penetration to resolve

		// Calculate the total mass of the particles involved in the contact
		float totalMass = (float)1 / particles[0]->mass;
		if (particles[1]) totalMass += (float)1 / particles[1]->mass; // Calculate total mass


		if (totalMass <= 0.0f) return; // No mass to resolve

		// Calculate the amount to move each particle based on its mass
		float totalMoveByMass = depth / totalMass;

		// Calculate the move vector based on the contact normal and the total move by mass
		MyVector moveByMass = contactNormal * totalMoveByMass;
		
		// Apply the move vector to each particle based on its mass
		MyVector P_a = moveByMass * ((float)1 / particles[0]->mass);

		// Update the position of the first particle
		particles[0]->position += P_a;

		// If there is a second particle, apply the move vector to it as well
		if (particles[1]) {
			MyVector P_b = moveByMass * (-(float)1 / particles[1]->mass);
			particles[1]->position += P_b;
		}

		depth = 0.0f; // Reset the depth after resolving the interpenetration
	}
}