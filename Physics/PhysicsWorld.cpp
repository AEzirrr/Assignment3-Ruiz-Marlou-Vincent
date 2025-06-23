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

	void PhysicsWorld::addContact(P6Particle* p1, P6Particle* p2, float restitution, MyVector contactNormal) {
		ParticleContact *toAdd = new ParticleContact();

		toAdd->particles[0] = p1;
		toAdd->particles[1] = p2;
		toAdd->restitution = restitution;
		toAdd->contactNormal = contactNormal; 
		toAdd->depth = (p1->position - p2->position).Magnitude(); // Calculate the depth of penetration
		contacts.push_back(toAdd);
	}

	void PhysicsWorld::GenerateContact() {
		
		contacts.clear();

		for (std::list<ParticleLink*>::iterator i = links.begin();
			i != links.end();
			i++) {

			ParticleContact* contact = (*i)->GetContact();

			if (contact != nullptr) {
				contacts.push_back(contact);
			}
		}
	}
}