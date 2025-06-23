#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF
#include "ForceGenerator.h"

#endif

#include "P6Particle.h"

/*
The Bungee class is similar to the AnchoredSpring class,
but it only applies force when the particle is stretched
over rhe rest length. the force that is applied is proportional to the
amount of stretch, and pulls the particle back.
*/

namespace Physics {
	class Bungee : public ForceGenerator {

	private:
		float bungeeConstant; 
		float restLength; 

	public:
		MyVector anchorPoint;
		float damping;

		Bungee(MyVector pos, float bungeeConst, float restLen)
			: anchorPoint(pos), bungeeConstant(bungeeConst), restLength(restLen) {
		}

		void UpdateForce(P6Particle* particle, float time) override;

	};
}
