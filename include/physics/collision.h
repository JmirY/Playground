#ifndef COLLISION_H
#define COLLISION_H

#include "rigid_body.h"

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

    class Sphere : public Collider
    {
        friend class CollisionDetector;

    protected:
        /* 구의 반지름 */
        float radius;

    public:
        /* 생성자 */
        Sphere(RigidBody* body, float radius);
    };

    class Box : public Collider
    {
        friend class CollisionDetector;

    protected:
        /* x, y, z 축과 평행한 변의 절반 */
        float halfX;
        float halfY;
        float halfZ;

    public:
        /* 생성자 */
        Box(RigidBody* body, float halfX, float halfY, float halfZ);
    };

    /* 바닥, 벽을 표현하기 위한 반무한체.
        오브젝트는 평면을 통과해 반대편으로 넘어갈 수 없다 */
    class Plane
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
        Plane(Vector3 normal, float offset);
    };

    class CollisionDetector
    {
    public:
        static bool sphereAndBox(const Sphere&, const Box&);
        static bool sphereAndSphere(const Sphere&, const Sphere&);
        static bool sphereAndPlane(const Sphere&, const Plane&);
        static bool boxAndBox(const Box&, const Box&); // Seperating Axis Theorem 사용
        static bool boxAndPlane(const Box&, const Plane&);

        /* 두 박스가 주어진 축에 대해 어느정도 겹치는지 반환한다 */
        static float calcPenetration(const Box& box1, const Box& box2, const Vector3& axis);
    };
} // namespace physics


#endif // COLLISION_H
