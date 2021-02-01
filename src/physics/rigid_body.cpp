#include <physics/rigid_body.h>
#include <cmath>

using namespace physics;

void RigidBody::integrate(float duration)
{
    /* 강체의 질량이 무한대라면 적분을 하지 않는다 */
    if (inverseMass == 0.0f)
        return;
    
    /* 위치를 업데이트한다 */
    position += velocity * duration;

    /* 속도를 업데이트한다 */
    velocity = velocity * powf(linearDamping, duration) + acceleration * duration;
}

void RigidBody::setMass(float value)
{
    inverseMass = 1.0f / value;
}

void RigidBody::setInverseMass(float value)
{
    inverseMass = value;
}

void RigidBody::setPosition(const Vector3& vec)
{
    position = vec;
}

void RigidBody::setPosition(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
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

Vector3 RigidBody::getPosition() const
{
    return position;
}

Vector3 RigidBody::getVelocity() const
{
    return velocity;
}

Vector3 RigidBody::getAcceleration() const
{
    return acceleration;
}

float RigidBody::getLinearDamping() const
{
    return linearDamping;
}
