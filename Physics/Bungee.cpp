#include "Bungee.h"

namespace Physics
{
    void Bungee::UpdateForce(P6Particle* particle, float time) {
        MyVector force = particle->position - anchorPoint;

        float mag = force.Magnitude();

        if (mag <= restLength || mag == 0) return;

        force = force.Normalize();

        float bungeeForce = -bungeeConstant * (mag - restLength);
        force = force * bungeeForce;

        MyVector relativeVel = particle->velocity; 
        MyVector dampingVal = relativeVel * -damping;

        particle->AddForce(force + dampingVal);
    }

}
