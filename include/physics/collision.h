#ifndef COLLISION_H
#define COLLISION_H

#include "body.h"
#include "contact.h"
#include "../geometry.h"
#include <vector>

namespace physics
{
    /* 전방 선언 */
    class CollisionDetector; // Collider 클래스 내 friend 선언을 위함
    
    /* 충돌 감지를 위한 충돌체.
        충돌 가능한 모든 게임 오브젝트는 이 클래스의 참조를 속성으로 가진다.
        충돌체의 속성은 게임 오브젝트로부터 입력받는다 */
    class Collider
    {
    public:
        friend class CollisionDetector;
        friend class Simulator;

    protected:
        Geometry geometry;
        RigidBody* body;

    public:
        virtual ~Collider() {}
        virtual void setGeometricData(double, ...) = 0;
    };

    class SphereCollider : public Collider
    {
        friend class CollisionDetector;
        friend class Simulator;

    protected:
        float radius;

    public:
        SphereCollider(RigidBody* body, float radius);

        void setGeometricData(double, ...);
    };

    class BoxCollider : public Collider
    {
        friend class CollisionDetector;
        friend class Simulator;

    protected:
        float halfX;
        float halfY;
        float halfZ;

    public:
        BoxCollider(RigidBody* body, float halfX, float halfY, float halfZ);

        void setGeometricData(double, ...);
    };

    /* 바닥, 벽을 표현하기 위한 반무한체.
        오브젝트는 평면을 통과해 반대편으로 넘어갈 수 없다 */
    class PlaneCollider
    {
        friend class CollisionDetector;

    protected:
        /* 평면의 법선.
            크기는 항상 1 이다 */
        Vector3 normal;

        /* 법선 방향으로 원점으로부터 얼마나 멀어져 있는지 저장한다 */
        float offset;

    public:
        PlaneCollider(Vector3 normal, float offset);
    };

    /* 충돌체 사이에 충돌이 일어났는지 검사하는 클래스 */
    class CollisionDetector
    {
    public:
        /* 충돌 검사 함수들.
            충돌이 있다면 충돌 정보 구조체를 생성하고 contacts 에 푸쉬하고 true 를 반환한다.
            총돌이 없다면 false 를 반환한다 */
        static bool sphereAndBox(
            std::vector<Contact*>& contacts,
            const SphereCollider&,
            const BoxCollider&
        );
        static bool sphereAndSphere(
            std::vector<Contact*>& contacts,
            const SphereCollider&,
            const SphereCollider&
        );
        static bool sphereAndPlane(
            std::vector<Contact*>& contacts,
            const SphereCollider&,
            const PlaneCollider&
        );
        /* Seperating Axis Theorem 사용 */
        static bool boxAndBox(
            std::vector<Contact*>& contacts,
            const BoxCollider&,
            const BoxCollider&
        );
        static bool boxAndPlane(
            std::vector<Contact*>& contacts,
            const BoxCollider&,
            const PlaneCollider&
        );
    
    private:
        /* 두 박스가 주어진 축에 대해 어느정도 겹치는지 반환한다 */
        static float calcPenetration(
            const BoxCollider& box1,
            const BoxCollider& box2,
            const Vector3& axis
        );
        
        /* 직육면체의 면-점 접촉일 때 충돌점을 찾는다 */
        static Vector3 calcContactPointOnPlane(
            const BoxCollider& box1,
            const BoxCollider& box2,
            const Vector3& contactNormal,
            int minPenetrationAxisIdx
        );

        /* 직육면체의 선-선 접촉일 때 충돌점을 찾는다 */
        static Vector3 calcContactPointOnLine(
            const BoxCollider& box1,
            const BoxCollider& box2,
            const Vector3& contactNormal,
            int minPenetrationAxisIdx
        );
    };
} // namespace physics


#endif // COLLISION_H
