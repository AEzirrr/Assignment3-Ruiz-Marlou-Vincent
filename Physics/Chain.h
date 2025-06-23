#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF
#include "ForceGenerator.h"

#endif

#include "P6Particle.h"

/*
The Chain class is also similar to the AnchoredSpring class, 
but the particle stops once it has reached its rest length 
with no more additional force and the chain does not stretch
unless we add extra force on it to show a taut rather than a some elasticity.
*/
namespace Physics {
	class Chain : public ForceGenerator {

	private: 
		float chainConstant; 
		float restLength; 

	public:
		MyVector anchorPoint;

		Chain(MyVector pos, float _chainConst, float restLen)
			: anchorPoint(pos), chainConstant(_chainConst), restLength(restLen) {
		}

		void UpdateForce(P6Particle* particle, float time) override;
	};
}
