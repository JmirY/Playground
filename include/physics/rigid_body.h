#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "vector3.h"

namespace physics
{
    class RigidBody
    {
    protected:
        /* 질량의 역수.
            적분식에 활용하기 좋고,
            무한대의 질량(ex. 배경)을 표현하기 편하다 */
        float inverseMass;

        /* 강체의 위치 */
        Vector3 position;

        /* 강체의 속도 */
        Vector3 velocity;

        /* 강체의 가속도 */
        Vector3 acceleration;

        /* 선속도 댐핑 */
        float linearDamping;

    public:
        /* 생성자 */
        RigidBody() : linearDamping(0.99f) {}

        /* 주어진 시간이 흘렀을 때, 강체의 상태를 계산 및 갱신한다 */
        void integrate(float duration);

        /* setter */
        void setMass(float value); // 함수 내부에서 질량의 역수로 바꿔 저장한다
        void setInverseMass(float value);

        void setPosition(const Vector3& vec);
        void setPosition(float x, float y, float z);
        
        void setVelocity(const Vector3& vec);
        void setVelocity(float x, float y, float z);

        void setAcceleration(const Vector3& vec);
        void setAcceleration(float x, float y, float z);

        void setLinearDamping(float value);

        /* getter */
        float getMass() const;
        float getInverseMass() const;
        Vector3 getPosition() const;
        Vector3 getVelocity() const;
        Vector3 getAcceleration() const;
        float getLinearDamping() const;
    }; 
} // namespace physics

#endif // RIGID_BODY_H
