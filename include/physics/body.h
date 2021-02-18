#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "vector3.h"
#include "matrix3.h"
#include "matrix4.h"
#include "quaternion.h"

namespace physics
{
    class RigidBody
    {
    protected:        
        /* 질량의 역수.
            적분식에 활용하기 좋고,
            무한대의 질량(ex. 배경)을 표현하기 편하다 */
        float inverseMass;

        /* 관성 모멘트 역텐서 */
        Matrix3 inverseInertiaTensor;      // 로컬 좌표계 기준
        Matrix3 inverseInertiaTensorWorld; // 월드 좌표계 기준

        /* 강체의 위치 */
        Vector3 position;

        /* 강체가 바라보는 방향 */
        Quaternion orientation;

        /* 강체의 속도 */
        Vector3 velocity;

        /* 강체의 각속도 */
        Vector3 rotation;

        /* 강체의 가속도.
            보통 중력 가속도만을 저장한다 */
        Vector3 acceleration;

        /* 직전 프레임에서 강체의 가속도.
            중력 가속도 + 외부 힘에 의한 가속도를 저장한다 */
        Vector3 prevAcceleration;

        /* 선속도 댐핑 */
        float linearDamping;

        /* 각속도 댐핑 */
        float angularDamping;

        /* 로컬 -> 월드 변환 행렬 */
        Matrix4 transformMatrix;

        /* 강체에 가해진 힘 & 토크 */
        Vector3 force;
        Vector3 torque;

    public:
        /* 생성자 */
        RigidBody() : linearDamping(0.99f), angularDamping(0.99f) {}

        /* 주어진 시간이 흘렀을 때, 강체의 상태를 계산 및 갱신한다 */
        void integrate(float duration);

        /* 힘을 강체 위의 점에 적용한다.
            인자는 모두 월드 좌표계 기준이다 */
        void addForceAt(const Vector3& force, const Vector3& point);

        /* 로컬 좌표계의 x, y, z 축 중 하나를 월드 좌표계로 변환하고
            단위 벡터화하여 반환한다. */
        Vector3 getAxis(int index) const;

    private:
        /* 강체에 가해진 힘과 토크를 0 으로 설정한다 */
        void clearForceAndTorque();
    
        /* 현재 상태를 참고하여 변환 행렬을 업데이트한다 */
        void updateTransformMatrix();

        /* 관성 모멘트 텐서를 로컬 -> 월드 좌표계 기준으로 변환한다 */
        void transformInertiaTensor();

    public:
        /* setter */
        void setMass(float value); // 함수 내부에서 질량의 역수로 바꿔 저장한다
        void setInverseMass(float value);

        void setInertiaTensor(const Matrix3& mat);

        void setPosition(const Vector3& vec);
        void setPosition(float x, float y, float z);
        
        void setVelocity(const Vector3& vec);
        void setVelocity(float x, float y, float z);

        void setRotation(const Vector3& vec);
        void setRotation(float x, float y, float z);

        void setAcceleration(const Vector3& vec);
        void setAcceleration(float x, float y, float z);

        void setLinearDamping(float value);

        /* getter */
        float getMass() const;
        float getInverseMass() const;
        Matrix3 getInverseInertiaTensor() const;
        Matrix3 getInverseInertiaTensorWorld() const;
        Vector3 getPosition() const;
        Vector3 getVelocity() const;
        Vector3 getRotation() const;
        Vector3 getAcceleration() const;
        float getLinearDamping() const;

        void getTransformMatrix(float matrix[16]) const;
        Matrix4 getTransformMatrix() const;
    }; 
} // namespace physics

#endif // RIGID_BODY_H
