#pragma once
#include <list>

#ifndef P6PARTICLE_DEF
#define P6PARTICLE_DEF

#include "P6Particle.h"

#endif 

#include <vector>
#include "ParticleContact.h"
#include "ParticleLink.h"
#include "ContactResolver.h"
#include "ForceRegistry.h"
#include "GravityForceGenerator.h"

namespace Physics {
	class PhysicsWorld
	{
	protected:

		void GenerateContact();

		ContactResolver contactResolver = ContactResolver(20); 

	public:

		std::vector<ParticleContact*> contacts;

		std::list<ParticleLink*> links;

		void addContact(P6Particle* p1, P6Particle* p2, float resitution, MyVector contactNormal);

		ForceRegistry forceRegistry;

		std::list<P6Particle*> particles;

		void AddParticle(P6Particle* toAdd);

		void Update(float time);

	private:
		void UpdateParticleList();

		GravityForceGenerator gravity = GravityForceGenerator(MyVector(0, -9.8f, 0));
	};
}
