#ifndef RESOLVER_H
#define RESOLVER_H

#include "contact.h"

namespace physics
{
    class CollisionResolver
    {
    public:
        void resolveContact(Contact*);

    public:
        void resolveVelocity(Contact*);
        void resolvePenetration(Contact*);
    
        Matrix3 calcWorldToContactMat(const Vector3& contactNormal);
        float calcDeltaVelocityPerUnitImpulse(
            RigidBody* (&bodies)[2],
            const Vector3& contactNoraml,
            const Vector3& contactPoint,
            const Matrix3& worldToContact
        );
        float calcDeltaVelocity(
            RigidBody* (&bodies)[2],
            const Vector3& contactPoint,
            const float restitution,
            const Matrix3& worldToContact
        );
    };
} // namespace physics


#endif // RESOLVER_H
