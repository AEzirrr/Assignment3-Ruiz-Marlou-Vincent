#pragma once

#ifndef P6PARTICLE_DEF
#define P6PARTICLE_DEF

#include "P6Particle.h"

#endif 

namespace Physics{

	class ParticleContact{
	public:

		P6Particle* particles[2]; // The two particles involved in the contact

		float restitution; // The restitution coefficient for the contact

		float depth; // The depth of the contact, how far the particles are penetrating each other

		MyVector contactNormal; // The normal vector at the contact point

		float GetSeparatingSpeed();

		void Resolve(float time);

	protected:

		void ResolveVelocity(float time);

		void ResolveInterpenetration(float time);
	};
}




