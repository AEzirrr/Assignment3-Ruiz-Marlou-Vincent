#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF
#include "ForceGenerator.h"

#endif

#include "P6Particle.h"


//Cable class is very much the same as the AnchoredSpring class, 
//but it is used to simulate a cable that is anchored at a point in space. It can move freely but does not stretch beyond its rest length.
namespace Physics {
	class Cable : public ForceGenerator {

	private: 
		float chainConstant; 
		float restLength; 

	public:
		MyVector anchorPoint;

		Cable(MyVector pos, float _chainConst, float restLen)
			: anchorPoint(pos), chainConstant(_chainConst), restLength(restLen) {
		}

		void UpdateForce(P6Particle* particle, float time) override;
	};
}
