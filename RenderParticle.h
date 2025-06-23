#pragma once
#include "Physics/P6Particle.h"
#include "model.h"	

class RenderParticle
{
	public:

		Physics::P6Particle* PhysicsParticle;

		Model3D* RenderObject;

		Physics::MyVector Color;

		RenderParticle(Physics::P6Particle* p, Model3D* obj) : PhysicsParticle(p), RenderObject(obj) {

			Color = Physics::MyVector(1, 1, 1);
		};

		RenderParticle(Physics::P6Particle* p, Model3D* obj, Physics::MyVector c) : PhysicsParticle(p), RenderObject(obj), Color(c) {};

		void Draw();
};