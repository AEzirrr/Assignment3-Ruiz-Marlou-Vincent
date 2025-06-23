#include "ContactResolver.h"
#include <iostream>

namespace Physics
{

    void ContactResolver::ResolveContacts(std::vector<ParticleContact*> contacts, float time)
    {
        int resolveCount = 0;

        while (resolveCount < max_iterations)
        {
            ParticleContact* bestContact = nullptr;
            float leastSeparatingSpeed = std::numeric_limits<float>::max();

            for (auto contact : contacts)
            {
                float sepSpeed = contact->GetSeparatingSpeed();

                if (sepSpeed < leastSeparatingSpeed || contact->depth > 0.0f)
                {
                    bestContact = contact;
                    leastSeparatingSpeed = sepSpeed;
                }
            }

            if (!bestContact || (bestContact->GetSeparatingSpeed() >= 0.0f || bestContact->depth <= 0.0f))
            {
                float printSepSpeed = bestContact->GetSeparatingSpeed();
				std::cout << printSepSpeed << "\n";
                break;
            }

            bestContact->Resolve(time);
            ++resolveCount;
        }
    }
}
