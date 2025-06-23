#pragma once
#include "ParticleContact.h"

namespace Physics {
    class ParticleLink {
    public:
        P6Particle* particles[2];

        virtual ParticleContact* GetContact() { return nullptr; };

    protected:
        float CurrentLength();
    };
}
