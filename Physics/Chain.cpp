#include "Chain.h"

namespace Physics
{
    void Chain::UpdateForce(P6Particle* particle, float time) {
        MyVector direction = particle->position - anchorPoint;
        float distance = direction.Magnitude();

        if (distance <= restLength) return;

        direction = direction.Normalize();
        particle->position = anchorPoint + direction * restLength;

        float vOut = particle->velocity.ScalarProduct(direction);
        if (vOut > 0) {
            particle->velocity = particle->velocity - direction * vOut;
        }
    }

}
