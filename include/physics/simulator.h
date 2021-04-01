#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "body.h"
#include "detector.h"
#include "resolver.h"
#include "../playground/geometry.h"
#include "../playground/contact_info.h"
#include <vector>
#include <map>

namespace physics
{
    class Simulator
    {
    public:
        typedef std::map<unsigned int, RigidBody*> RigidBodies;
        typedef std::map<unsigned int, Collider*> Colliders;
        typedef std::vector<Contact*> Contacts;

    private:
        RigidBodies bodies;
        Colliders colliders;
        PlaneCollider groundCollider;
        Contacts contacts;
        
        CollisionDetector detector;
        CollisionResolver resolver;

    public:
        Simulator() : groundCollider(Vector3(0.0f, 1.0f, 0.0f), 0.0f) {}
        ~Simulator();

        /* 주어진 시간 동안의 물리 현상을 시뮬레이팅한다 */
        void simulate(float duration, std::vector<ContactInfo*>&);

        /* 새로운 강체를 시뮬레이션에 추가하고 추가된 강체의 주소를 반환한다 */
        RigidBody* addRigidBody(unsigned int id, Geometry);

        /* 주어진 강체를 감싸는 충돌체를 시뮬레이션에 추가하고 추가된 충돌체의 주소를 반환한다 */
        Collider* addCollider(unsigned int id, Geometry, RigidBody*);

        /* 충돌체와 강체를 제거한다 */
        void removePhysicsObject(unsigned int id);

        float calcDistanceBetweenRayAndObject(
            const Vector3& rayOrigin,
            const Vector3& rayDirection,
            const unsigned int id
        );

        void getContactInfo(std::vector<ContactInfo*>&) const;

        void setGroundRestitution(float value);
        void setObjectRestitution(float value);

    private:
        /* 충돌을 검출하고 충돌 정보를 contacts 에 저장한다 */
        void detectCollision();
    };
} // namespace physics

#endif // SIMULATOR_H
