#include <physics/body.h>
#include <cmath>
#include <iostream>

using namespace physics;

void RigidBody::integrate(float duration)
{
    /* 강체의 질량이 무한대라면 적분을 하지 않는다 */
    if (inverseMass == 0.0f)
        return;

    /* 가속도를 계산한다 */
    prevAcceleration = acceleration;
    prevAcceleration += force * inverseMass;

    /* 각가속도를 계산한다 */
    Vector3 angularAcceleration = inverseInertiaTensorWorld * torque;

    /* 속도 & 각속도를 업데이트한다 */
    velocity += prevAcceleration * duration;
    rotation += angularAcceleration * duration;

    /* 드래그를 적용한다 */
    velocity *= powf(linearDamping, duration);
    rotation *= powf(angularDamping, duration);

    /* 위치 & 방향을 업데이트한다 */
    position += velocity * duration;
    orientation += orientation.rotateByScaledVector(rotation, duration / 2.0f);

    /* 사원수를 정규화한다 */
    orientation.normalize();

    /* 변화된 상태에 따라 변환 행렬을 업데이트한다 */
    updateTransformMatrix();

    /* 월드 좌표계 기준의 관성 텐서를 업데이트한다 */
    transformInertiaTensor();

    /* 강체에 적용된 힘과 토크는 제거한다 */
    clearForceAndTorque();
}

void RigidBody::addForceAt(const Vector3& _force, const Vector3& point)
{
    /* 힘을 업데이트한다 */
    force += _force;
    /* 토크를 업데이트한다 */
    Vector3 pointFromCenter = point - position;
    torque += pointFromCenter.cross(_force);
}

Vector3 RigidBody::getAxis(int index) const
{
    /* 입력값 검사 */
    if (index < 0 || index > 3)
    {
        std::cout << "RigidBody::getAxis::Out of index" << std::endl;
        return Vector3();
    }

    Vector3 result(
        transformMatrix.entries[index],
        transformMatrix.entries[index + 4],
        transformMatrix.entries[index + 8]
    );
    result.normalize();

    return result;
}

void RigidBody::clearForceAndTorque()
{
    force = torque = Vector3(0.0f, 0.0f, 0.0f);
}

void RigidBody::updateTransformMatrix()
{
    transformMatrix.entries[0] = 
        1.0f - 2.0f * (orientation.y*orientation.y + orientation.z*orientation.z);
    transformMatrix.entries[1] = 
        2.0f * (orientation.x*orientation.y - orientation.w*orientation.z);
    transformMatrix.entries[2] = 
        2.0f * (orientation.x*orientation.z + orientation.w*orientation.y);
    transformMatrix.entries[3] = position.x;

    transformMatrix.entries[4] =
        2.0f * (orientation.x*orientation.y + orientation.w*orientation.z);
    transformMatrix.entries[5] =
        1.0f - 2.0f * (orientation.x*orientation.x + orientation.z*orientation.z);
    transformMatrix.entries[6] =
        2.0f * (orientation.y*orientation.z - orientation.w*orientation.x);
    transformMatrix.entries[7] = position.y;

    transformMatrix.entries[8] =
        2.0f * (orientation.x*orientation.z - orientation.w*orientation.y);
    transformMatrix.entries[9] =
        2.0f * (orientation.y*orientation.z + orientation.w*orientation.x);
    transformMatrix.entries[10] =
        1.0f - 2.0f * (orientation.x*orientation.x + orientation.y*orientation.y);
    transformMatrix.entries[11] = position.z;
}

void RigidBody::transformInertiaTensor()
{
    /* 변환 행렬 중 회전 변환 행렬만 뽑아낸다 */
    Matrix3 rotationMatrix;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            rotationMatrix.entries[3*i + j] = transformMatrix.entries[4*i + j];

    inverseInertiaTensorWorld = (rotationMatrix * inverseInertiaTensor) * rotationMatrix.transpose();
}

void RigidBody::setMass(float value)
{
    inverseMass = 1.0f / value;
}

void RigidBody::setInverseMass(float value)
{
    inverseMass = value;
}

void RigidBody::setInertiaTensor(const Matrix3& mat)
{
    inverseInertiaTensor = mat.inverse();
    transformInertiaTensor();
}

void RigidBody::setPosition(const Vector3& vec)
{
    position = vec;
    updateTransformMatrix();
}

void RigidBody::setPosition(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
    updateTransformMatrix();
}

void RigidBody::setOrientation(const Quaternion& quat)
{
    orientation = quat;
    updateTransformMatrix();
    transformInertiaTensor();
}

void RigidBody::setVelocity(const Vector3& vec)
{
    velocity = vec;
}

void RigidBody::setVelocity(float x, float y, float z)
{
    velocity.x = x;
    velocity.y = y;
    velocity.z = z;
}

void RigidBody::setRotation(const Vector3& vec)
{
    rotation = vec;
}

void RigidBody::setRotation(float x, float y, float z)
{
    rotation.x = x;
    rotation.y = y;
    rotation.z = z;
}

void RigidBody::setAcceleration(const Vector3& vec)
{
    acceleration = vec;
}

void RigidBody::setAcceleration(float x, float y, float z)
{
    acceleration.x = x;
    acceleration.y = y;
    acceleration.z = z;
}

void RigidBody::setLinearDamping(float value)
{
    linearDamping = value;
}

float RigidBody::getMass() const
{
    return 1.0f / inverseMass;
}

float RigidBody::getInverseMass() const
{
    return inverseMass;
}

Matrix3 RigidBody::getInverseInertiaTensor() const
{
    return inverseInertiaTensor;
}

Matrix3 RigidBody::getInverseInertiaTensorWorld() const
{
    return inverseInertiaTensorWorld;
}

Vector3 RigidBody::getPosition() const
{
    return position;
}

Vector3 RigidBody::getVelocity() const
{
    return velocity;
}

Vector3 RigidBody::getRotation() const
{
    return rotation;
}

Vector3 RigidBody::getAcceleration() const
{
    return acceleration;
}

float RigidBody::getLinearDamping() const
{
    return linearDamping;
}

void RigidBody::getTransformMatrix(float matrix[16]) const
{
    matrix[0] = transformMatrix.entries[0];
    matrix[1] = transformMatrix.entries[4];
    matrix[2] = transformMatrix.entries[8];
    matrix[3] = transformMatrix.entries[12];

    matrix[4] = transformMatrix.entries[1];
    matrix[5] = transformMatrix.entries[5];
    matrix[6] = transformMatrix.entries[9];
    matrix[7] = transformMatrix.entries[13];

    matrix[8] = transformMatrix.entries[2];
    matrix[9] = transformMatrix.entries[6];
    matrix[10] = transformMatrix.entries[10];
    matrix[11] = transformMatrix.entries[14];

    matrix[12] = transformMatrix.entries[3];
    matrix[13] = transformMatrix.entries[7];
    matrix[14] = transformMatrix.entries[11];
    matrix[15] = transformMatrix.entries[15];
}

Matrix4 RigidBody::getTransformMatrix() const
{
    return transformMatrix;
}
