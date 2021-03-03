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

void Object::getAccelerationInArray(float (&array)[3]) const
{
    physics::Vector3 acceleration = body->getAcceleration();
    array[0] = acceleration.x;
    array[1] = acceleration.y;
    array[2] = acceleration.z;
}

void SphereObject::setGeometricData(double value, ...)
{
    radius = value;
}

void SphereObject::updateDerivedData()
{
    /* 강체의 데이터를 갱신한다 */
    physics::Matrix3 inertiaTensor;
    inertiaTensor.setDiagonal(0.4f * body->getMass() * radius*radius);
    body->setInertiaTensor(inertiaTensor);

    /* 충돌체의 데이터를 갱신한다 */
    collider->setGeometricData(radius);

    /* Shape 의 데이터를 갱신한다 */
    shape->generateVertices(radius);
    shape->generateVAOs();
}

void BoxObject::getHalfSizeInArray(float (&array)[3]) const
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
    physics::Matrix3 inertiaTensor;
    float k = body->getMass() / 12;
    inertiaTensor.entries[0] = k * (halfY*halfY + halfZ*halfZ);
    inertiaTensor.entries[4] = k * (halfX*halfX + halfZ*halfZ);
    inertiaTensor.entries[8] = k * (halfY*halfY + halfX*halfX);
    body->setInertiaTensor(inertiaTensor);

    /* 충돌체의 데이터를 갱신한다 */
    collider->setGeometricData(halfX, halfY, halfZ);

    /* Shape 의 데이터를 갱신한다 */
    shape->generateVertices(halfX, halfY, halfZ);
    shape->generateVAOs();
}
