#ifndef P6PARTICLE_DEF
#define P6PARTICLE_DEF

#include "P6Particle.h"

#endif


namespace Physics {
	class ForceGenerator {
	public:
		virtual void UpdateForce(P6Particle* particle, float time) {
			particle->AddForce(MyVector(0, 0, 0));
		}
	};
}
