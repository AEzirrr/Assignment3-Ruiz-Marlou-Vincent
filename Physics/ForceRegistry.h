#ifndef P6PARTICLE_DEF
#define P6PARTICLE_DEF

#include "P6Particle.h"

#endif

#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF

#include "ForceGenerator.h"

#endif 

#include "list"

namespace Physics {
	class ForceRegistry {
	public:
		void Add(P6Particle* particle, ForceGenerator* generator);
		void Remove(P6Particle* particle, ForceGenerator* generator);
		void Clear();
		void UpdateForces(float time);

	protected:
		struct ParticleForceRegistry {
			P6Particle* particle;
			ForceGenerator* generator;
		};

		std::list<ParticleForceRegistry> registry;
	};
}