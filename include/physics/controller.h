#ifndef PHYSICS_CONTROLLER_H
#define PHYSICS_CONTROLLER_H

#include "body.h"
#include "collision.h"
#include "../geometry.h"
#include <vector>

namespace physics
{
    class PhysicsController
    {
    public:
        typedef std::vector<RigidBody*> RigidBodies;
        typedef std::vector<Collider*> Colliders;
        typedef std::vector<Contact*> Contacts;

    public: // TODO: private 으로 변경
        RigidBodies bodies;
        Colliders colliders;
        Contacts contacts;

        PlaneCollider groundCollider;

    public:
        /* 생성자 */
        PhysicsController() : groundCollider(Vector3(0.0f, 1.0f, 0.0f), 0.0f) {}

        /* 소멸자 */
        ~PhysicsController();

        /* 주어진 시간 동안의 물리 현상을 시뮬레이팅한다 */
        void simulate(float duration);

        /* 새로운 물체를 시뮬레이션에 추가한다 */
        void addPhysicsObject(Geometry);

    private:
        /* 충돌을 검출하고 충돌 정보를 contacts 에 저장한다 */
        void detectCollision();
    };
} // namespace physics

#endif // PHYSICS_CONTROLLER_H
