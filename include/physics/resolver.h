#ifndef RESOLVER_H
#define RESOLVER_H

#include "contact.h"
#include <vector>

namespace physics
{
    class CollisionResolver
    {
    private:
        int iterationLimit;
        float penetrationTolerance;
        float closingSpeedTolerance;

    public:
        CollisionResolver()
            : iterationLimit(10), penetrationTolerance(0.005f), closingSpeedTolerance(0.5f) {}
    
        void resolveCollision(std::vector<Contact*>&, float deltaTime);

    private:
        void sequentialImpulse(Contact*, float deltaTime);
    };
} // namespace physics


#endif // RESOLVER_H
