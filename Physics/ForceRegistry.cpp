#include "ForceRegistry.h"

namespace Physics
{
	void ForceRegistry::Add(P6Particle* particle, ForceGenerator* generator) {

		ParticleForceRegistry toAdd;
		
		toAdd.particle = particle;
		toAdd.generator = generator;

		registry.push_back(toAdd);
	}

	void ForceRegistry::Remove(P6Particle* particle, ForceGenerator* generator) {

		registry.remove_if(
			[particle, generator](ParticleForceRegistry reg) {
				return reg.particle == particle && reg.generator == generator;
			}
		);
	}

	void ForceRegistry::Clear() {
		registry.clear();
	}

	void ForceRegistry::UpdateForces(float time)
	{

		for (std::list<ParticleForceRegistry>::iterator i = registry.begin();
			i != registry.end();
			i++) {

			i->generator->UpdateForce(i->particle, time);

		}
	}
}