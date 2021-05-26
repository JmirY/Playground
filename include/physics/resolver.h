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
            : iterationLimit(30), penetrationTolerance(0.0005f), closingSpeedTolerance(0.005f) {}
    
        void resolveCollision(std::vector<Contact*>&, float deltaTime);

    private:
        void sequentialImpulse(Contact*, float deltaTime);
    };
} // namespace physics


#endif // RESOLVER_H
