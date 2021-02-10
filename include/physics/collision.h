#ifndef COLLISION_H
#define COLLISION_H

#include "rigid_body.h"
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
    protected:
        RigidBody* body;
    };

    class SphereCollider : public Collider
    {
        friend class CollisionDetector;

    protected:
        /* 구의 반지름 */
        float radius;

    public:
        /* 생성자 */
        SphereCollider(RigidBody* body, float radius);
    };

    class BoxCollider : public Collider
    {
        friend class CollisionDetector;

    protected:
        /* x, y, z 축과 평행한 변의 절반 */
        float halfX;
        float halfY;
        float halfZ;

    public:
        /* 생성자 */
        BoxCollider(RigidBody* body, float halfX, float halfY, float halfZ);
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
        /* 생성자 */
        PlaneCollider(Vector3 normal, float offset);
    };

    /* 충돌 정보를 저장하는 구조체. 충돌 처리에 활용된다 */
    struct Contact
    {
        RigidBody* bodies[2];
        Vector3 normal;
        Vector3 contactPoint;
        float penetration;
        float restitution;
    };

    /* 두 오브젝트 사이에 충돌이 일어났을 때 아래의 탄성 계수를 사용해 충돌 정보를 작성한다 */
    const float RESTITUTION_OBJECT = 0.5f; // 오브젝트 사이의 충돌
    const float RESTITUTION_PLANE = 0.3f;  // 오브젝트와 평면 사이의 충돌

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
        
        /* 직육면체 사이의 면-점 접촉일 때 충돌 지점을 찾는다 */
        static Vector3 calcContactPointOnPlane(
            const BoxCollider& box1,
            const BoxCollider& box2,
            const std::vector<Vector3>& axes,
            int minPenetrationAxisIdx
        );

        /* 직육면체 사이의 선-선 접촉일 때 충돌 지점을 찾는다 */
        static Vector3 calcContactPointOnLine(
            const BoxCollider& box1,
            const BoxCollider& box2,
            const std::vector<Vector3>& axes,
            int minPenetrationAxisIdx
        );
    };
} // namespace physics


#endif // COLLISION_H
