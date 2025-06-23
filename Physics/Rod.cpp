#include "Rod.h"

namespace Physics {

	ParticleContact* Rod::GetContact() {
		float currLen = CurrentLength();

		if (currLen == length) {
			return nullptr; // No contact if the length is correct
		}

		ParticleContact* ret = new ParticleContact();
		ret->particles[0] = particles[0];
		ret->particles[1] = particles[1];

		MyVector dir = particles[1]->position - particles[0]->position;
		dir = dir.Normalize();

		if (currLen > length) {
			ret->contactNormal = dir; // Push apart
			ret->depth = currLen - length;
		}
		else {
			ret->contactNormal = dir *  -1; // Pull together
			ret->depth = length - currLen;
		}

		ret->restitution = restitution;

		return ret;
	}
}
