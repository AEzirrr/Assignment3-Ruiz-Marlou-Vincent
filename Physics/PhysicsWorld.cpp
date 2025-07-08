#include "PhysicsWorld.h"

namespace Physics {

	void PhysicsWorld::AddParticle(P6Particle* toAdd) {
		particles.push_back(toAdd);
		forceRegistry.Add(toAdd, &gravity); // Add the particle to the force registry with gravity
	}

	void PhysicsWorld::Update(float time) {

		UpdateParticleList();

		forceRegistry.UpdateForces(time);

		for (std::list<P6Particle*>::iterator p = particles.begin();

			p != particles.end();

			++p

			)
		{
			(*p)->Update(time);
		}

		GenerateContact();

		if (contacts.size() > 0) {
			contactResolver.ResolveContacts(contacts, time);
		}
	}

	void PhysicsWorld::UpdateParticleList() {

		particles.remove_if(
			[](P6Particle* p) {
				return p->IsDestroyed();
			}
		);
	}

	void PhysicsWorld::addContact(P6Particle* p1, P6Particle* p2, float restitution, MyVector contactNormal, float depth) {
		ParticleContact* toAdd = new ParticleContact();

		toAdd->particles[0] = p1;
		toAdd->particles[1] = p2;
		toAdd->restitution = restitution;
		toAdd->contactNormal = contactNormal;
		toAdd->depth = depth;
		contacts.push_back(toAdd);
	}

	void PhysicsWorld::GenerateContact() {

		contacts.clear();

		GetOverlaps();

		for (std::list<ParticleLink*>::iterator i = links.begin();
			i != links.end();
			i++) {

			ParticleContact* contact = (*i)->GetContact();

			if (contact != nullptr) {
				contacts.push_back(contact);
			}
		}
	}

	void PhysicsWorld::GetOverlaps() {
		for (int i = 0; i < particles.size() - 1; i++) {
			std::list<P6Particle*>::iterator a = std::next(particles.begin(), i);

			for (int h = i + 1; h < particles.size(); h++) {
				std::list<P6Particle*>::iterator b = std::next(particles.begin(), h);

				MyVector mag2Vector = (*a)->position - (*b)->position;
				float mag2 = mag2Vector.SqMagnitude();

				float rad = (*a)->radius + (*b)->radius;
				float rad2 = rad * rad;

				if (mag2 < rad2) {
					MyVector dir = mag2Vector.Normalize();
					float r = rad2 - mag2;
					float depth = sqrt(r);

					float restitution = fmin((*a)->restitution, (*b)->restitution);

					addContact(*a, *b, restitution, dir, depth);
				}
			}
		}
	}
}