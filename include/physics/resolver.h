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

    public:
        CollisionResolver() : iterationLimit(10) {}
    
        void resolveCollision(std::vector<Contact*>&, float deltaTime);

    private:
        void sequentialImpulse(Contact*, float deltaTime);
    };
} // namespace physics


#endif // RESOLVER_H
