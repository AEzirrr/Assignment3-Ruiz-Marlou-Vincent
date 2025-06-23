#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF
#include "ForceGenerator.h"

#endif

#include "P6Particle.h"

namespace Physics {
	class AnchoredSpring : public ForceGenerator {

	private:
		MyVector anchorPoint; // The anchor point of the spring
		float springConstant; // The spring constant (k)
		float restLength; // The rest length of the spring

	public:

		AnchoredSpring(MyVector pos, float _springConst, float restLen)
			: anchorPoint(pos), springConstant(_springConst), restLength(restLen) {}

		void UpdateForce(P6Particle* particle, float time) override;
	};

}

