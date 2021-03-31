#include <playground/object.h>
#include <cstdarg>

void Object::getPositionInArray(float (&array)[3]) const
{
    physics::Vector3 position = body->getPosition();
    array[0] = position.x;
    array[1] = position.y;
    array[2] = position.z;
}

void Object::getVelocityInArray(float (&array)[3]) const
{
    physics::Vector3 velocity = body->getVelocity();
    array[0] = velocity.x;
    array[1] = velocity.y;
    array[2] = velocity.z;
}

void Object::getRotationInArray(float (&array)[3]) const
{
    physics::Vector3 rotation = body->getRotation();
    array[0] = rotation.x;
    array[1] = rotation.y;
    array[2] = rotation.z;
}

void Object::getAccelerationInArray(float (&array)[3]) const
{
    physics::Vector3 acceleration = body->getAcceleration();
    array[0] = acceleration.x;
    array[1] = acceleration.y;
    array[2] = acceleration.z;
}

void Object::getMassInArray(float (&array)[3]) const
{
    float mass = body->getMass();
    array[0] = mass;
}

void SphereObject::getGeometricDataInArray(float (&array)[3]) const
{
    array[0] = radius;
}

void SphereObject::setGeometricData(double value, ...)
{
    radius = value;
}

void SphereObject::updateDerivedData()
{
    /* 강체의 데이터를 갱신한다 */
    if (body->getInverseMass() != 0.0f)
    {
        physics::Matrix3 inertiaTensor;
        inertiaTensor.setDiagonal(0.4f * body->getMass() * radius*radius);
        body->setInertiaTensor(inertiaTensor);
    }

    /* 충돌체의 데이터를 갱신한다 */
    collider->setGeometricData(radius);

    /* Shape 의 데이터를 갱신한다 */
    shape->generateVertices(radius);
    shape->generateVAOs();
}

void BoxObject::getGeometricDataInArray(float (&array)[3]) const
{
    array[0] = halfX;
    array[1] = halfY;
    array[2] = halfZ;
}

void BoxObject::setGeometricData(double value, ...)
{
    halfX = value;

    va_list args;
    va_start(args, value);
    
    halfY = va_arg(args, double);
    halfZ = va_arg(args, double);

    va_end(args);
}

void BoxObject::updateDerivedData()
{
    /* 강체의 데이터를 갱신한다 */
    if (body->getInverseMass() != 0.0f)
    {
        physics::Matrix3 inertiaTensor;
        float k = body->getMass() / 12;
        float x = halfX * 2.0f;
        float y = halfY * 2.0f;
        float z = halfZ * 2.0f;
        inertiaTensor.entries[0] = k * (y*y + z*z);
        inertiaTensor.entries[4] = k * (x*x + z*z);
        inertiaTensor.entries[8] = k * (y*y + x*x);
        body->setInertiaTensor(inertiaTensor);
    }
    /* 충돌체의 데이터를 갱신한다 */
    collider->setGeometricData(halfX, halfY, halfZ);

    /* Shape 의 데이터를 갱신한다 */
    shape->generateVertices(halfX, halfY, halfZ);
    shape->generateVAOs();
}
