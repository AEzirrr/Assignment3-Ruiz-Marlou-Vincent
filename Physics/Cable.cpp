#include "Cable.h"

namespace Physics
{
    void Cable::UpdateForce(P6Particle* particle, float time) {

        MyVector direction = particle->position - anchorPoint;
		float distance = direction.Magnitude(); //distance from the particle to the anchor point

		if (distance <= restLength) return; // if the particle is less than or equal to the rest length, no force is applied

		direction = direction.Normalize(); // normalize the direction vector

		particle->position = anchorPoint + direction * restLength; // Move the particle back to the rest length from the anchor

        float vOut = particle->velocity.ScalarProduct(direction);

        if (vOut > 0) { 
            particle->velocity = particle->velocity - direction * vOut; // cancel out the velocity so that the particle does not move beyond its rest length
        }
    }

}
