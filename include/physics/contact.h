#ifndef CONTACT_H
#define CONTACT_H

#include "body.h"

namespace physics
{
    /* 충돌 정보를 저장하는 구조체. 충돌 처리에 활용된다 */
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
        /* 속도 변화 & 겹침 처리를 수행한다 */
        void resolve();
    
    private:
        /* 충돌 후 두 물체의 속도를 계산하고 적용한다 */
        void resolveVelocity();

        /* 두 물체 사이의 겹침을 처리한다 */
        void resolvePenetration();
    };

    /* 두 오브젝트 사이에 충돌이 일어났을 때 아래의 탄성 계수를 사용해 충돌 정보를 작성한다 */
    const float RESTITUTION_OBJECT = 0.5f; // 오브젝트 사이의 충돌
    const float RESTITUTION_PLANE = 0.3f;  // 오브젝트와 평면 사이의 충돌
} // namespace physics

#endif // CONTACT_H
