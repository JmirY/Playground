#ifndef CONTACT_H
#define CONTACT_H

#include "body.h"

namespace physics
{
    /* 충돌 정보를 저장하는 구조체 */
    class Contact
    {
    public:
        friend class CollisionDetector;

    private:
        RigidBody* bodies[2];
        Vector3 normal;
        Vector3 contactPoint;
        float penetration;
        float restitution;
        float friction;

    public:
        /* 충돌 후 두 물체의 속도를 계산하고 적용한다 */
        void resolveVelocity();

        /* 두 물체 사이의 겹침을 처리한다 */
        void resolvePenetration();
    };
} // namespace physics

#endif // CONTACT_H
