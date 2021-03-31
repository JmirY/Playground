#ifndef COLLISION_H
#define COLLISION_H

#include "body.h"
#include "contact.h"
#include "../playground/geometry.h"
#include <vector>

namespace physics
{    
    /* 충돌 감지를 위한 충돌체.
        충돌 가능한 모든 게임 오브젝트는 이 클래스의 참조를 속성으로 가진다.
        충돌체의 속성은 게임 오브젝트로부터 입력받는다 */
    class Collider
    {
    public:
        friend class CollisionDetector;
        friend class Simulator;

    protected:
        RigidBody* body;

    public:
        Collider() {}
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
        SphereCollider(RigidBody* _body, float _radius);
        void setGeometricData(double, ...);
    };

    class BoxCollider : public Collider
    {
        friend class CollisionDetector;
        friend class Simulator;

    protected:
        Vector3 halfSize;

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

} // namespace physics


#endif // COLLISION_H
