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
        Vector3* contactPoint[2];
        float penetration;
        float restitution;
        float friction;
        /* 누적 충격량 (충돌 해소 시 사용) */
        float normalImpulseSum;
        float tangentImpulseSum1;
        float tangentImpulseSum2;
    };
} // namespace physics

#endif // CONTACT_H
