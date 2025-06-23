#include "ParticleLink.h"

namespace Physics
{
	float ParticleLink::CurrentLength() {

		MyVector ret = particles[0]->position - particles[1]->position;
		return ret.Magnitude();
	}
}
