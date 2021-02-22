#include <object.h>
#include <cstdarg>

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
    graphics::Sphere* sphereShape = static_cast<graphics::Sphere*>(shape);
    sphereShape->generateVertices(radius);
    sphereShape->generateVAOs();
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
    graphics::Box* boxShape = static_cast<graphics::Box*>(shape);
    boxShape->generateVertices(halfX, halfY, halfZ);
    boxShape->generateVAOs();
}
