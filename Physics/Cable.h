#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF
#include "ForceGenerator.h"

#endif

#include "P6Particle.h"

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
