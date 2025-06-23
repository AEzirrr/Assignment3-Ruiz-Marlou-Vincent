#pragma once
#include "ParticleLink.h"
#include "ParticleContact.h"

namespace Physics {
	class Rod : public ParticleLink {

	public:
		float length = 1;
		float restitution = 0.0f;

		ParticleContact* GetContact() override;
	};
}

