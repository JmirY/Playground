#ifndef CONTACT_H
#define CONTACT_H

#include "body.h"

namespace physics
{
    const float RESTITUTION_OBJECT = 0.5f; // 오브젝트 간 충돌의 탄성 계수
    const float RESTITUTION_PLANE = 0.3f;  // 오브젝트와 평면 간 충돌의 탄성 계수

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

    public:
        /* 충돌 후 두 물체의 속도를 계산하고 적용한다 */
        void resolveVelocity();

        /* 두 물체 사이의 겹침을 처리한다 */
        void resolvePenetration();
    };
} // namespace physics

#endif // CONTACT_H
