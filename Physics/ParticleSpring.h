#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF
#include "ForceGenerator.h"

#endif

#include "P6Particle.h"

namespace Physics {
	class ParticleSpring : public ForceGenerator
	{
	private:
		P6Particle* otherParticle; // The first particle in the spring
		float springConstant; // The spring constant (k)
		float restLength; // The rest length of the spring

	public:
		ParticleSpring(P6Particle* particle, float _springConst, float restLen)
			: otherParticle(particle), springConstant(_springConst), restLength(restLen) {
		}

		void UpdateForce(P6Particle* particle, float time) override;
	};
}

