#pragma once

#include "ParticleContact.h"
#include <vector>

namespace Physics {
	class ContactResolver
	{
	protected:
		unsigned current_iteration = 0;

	public:
		unsigned max_iterations;

		// Constructor declaration only
		ContactResolver(unsigned _maxIterations) : max_iterations(_maxIterations) {}

		void ResolveContacts(std::vector<ParticleContact*> contacts, float time);
	};
}
