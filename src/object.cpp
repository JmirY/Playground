#include <object.h>

void SphereObject::updateDerivedData()
{
    /* 강체의 데이터를 갱신한다 */
    physics::Matrix3 inertiaTensor;
    inertiaTensor.setDiagonal(0.4f * body->getMass() * radius*radius);
    body->setInertiaTensor(inertiaTensor);

    /* 충돌체의 데이터를 갱신한다 */
    physics::SphereCollider* sphereCollider = static_cast<physics::SphereCollider*>(collider);
    sphereCollider->setRadius(radius);

    /* Shape 의 데이터를 갱신한다 */
    graphics::Sphere* sphereShape = static_cast<graphics::Sphere*>(shape);
    sphereShape->generateVertices(radius);
    sphereShape->generateVAOs();
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
    physics::BoxCollider* boxCollider = static_cast<physics::BoxCollider*>(collider);
    boxCollider->setHalfSize(halfX, halfY, halfZ);

    /* Shape 의 데이터를 갱신한다 */
    graphics::Box* boxShape = static_cast<graphics::Box*>(shape);
    boxShape->generateVertices(halfX, halfY, halfZ);
    boxShape->generateVAOs();
}
