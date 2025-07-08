#pragma once
#include "MyVector.h"

namespace Physics {
    class P6Particle
    {
    protected:
		bool isDestroyed = false;

		MyVector accumulatedForce = MyVector(0, 0, 0);

        void UpdatePosition(float time);
        void UpdateVelocity(float time);

    public:
		P6Particle();

        void Update(float time);

        void AddForce(MyVector force);

		void ResetForce();

        void Destroy();

        bool IsDestroyed();

		float radius = 1.0f;

        float restitution = 0.9f;

		float damping = 0.9f; // Approximate drag

        float mass = 0;
        Physics::MyVector position;
        Physics::MyVector velocity;
        Physics::MyVector acceleration;
    };
}
