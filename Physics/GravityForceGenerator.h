#ifndef FORCEGENERATOR_DEF
#define FORCEGENERATOR_DEF
#include "ForceGenerator.h"

#endif


namespace Physics {
	class GravityForceGenerator : public ForceGenerator{
	private:
		MyVector Gravity = MyVector(0, -9.8f, 0);
	

	public:
		GravityForceGenerator(const MyVector gravity) : Gravity(gravity) {}

		void UpdateForce(P6Particle* particle, float time) override;
	};
}


