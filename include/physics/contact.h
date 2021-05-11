#ifndef CONTACT_H
#define CONTACT_H

#include "body.h"

namespace physics
{
    /* 충돌 정보를 저장하는 구조체 */
    struct Contact
    {
        RigidBody* bodies[2];
        Vector3 normal;
        Vector3 contactPoint;
        float penetration;
        float restitution;
        float friction;
    };
} // namespace physics

#endif // CONTACT_H
