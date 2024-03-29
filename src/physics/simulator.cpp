#include <physics/simulator.h>
#include <iterator>
#include <typeinfo>
#include <cmath>
#include <iostream>

using namespace physics;

Simulator::~Simulator()
{
    /* 충돌 정보 해제 */
    for (auto& contact : contacts)
        delete contact;
    
    /* 충돌체 해제 */
    for (auto& collider : colliders)
        delete collider.second;
    
    /* 강체 해제 */
    for (auto& body : bodies)
        delete body.second;
}

void Simulator::simulate(float duration, std::vector<ContactInfo*>& contactInfo)
{
    /* 물체들을 적분한다 */
    for (auto& body : bodies)
    {
        body.second->integrate(duration);
    }

    /* 물체 간 충돌을 검출한다 */
    detector.detectCollision(contacts, colliders, groundCollider);

    /* 충돌 정보를 복사한다 */
    getContactInfo(contactInfo);

    /* 충돌들을 처리한다 */
    resolver.resolveCollision(contacts, duration);
    for (auto& c : contacts)
    {
        for (auto& cp : c->contactPoint)
            delete cp;
        delete c;
    }
    contacts.clear();
}

RigidBody* Simulator::addRigidBody(unsigned int id, Geometry geometry, float posX, float posY, float posZ)
{
    /* 강체를 생성한다 */
    RigidBody* newBody = new RigidBody;
    newBody->setMass(5.0f);
    newBody->setPosition(posX, posY, posZ);
    newBody->setAcceleration(0.0f, -gravity, 0.0f);

    /* 강체의 관성 모멘트 텐서를 도형에 따라 결정한다 */
    Matrix3 inertiaTensor;
    if (geometry == SPHERE)
    {
        float value = 0.4f * newBody->getMass();
        inertiaTensor.setDiagonal(value);
    }
    else if (geometry == BOX)
    {
        float value = newBody->getMass() / 6.0f;
        inertiaTensor.setDiagonal(value);
    }
    newBody->setInertiaTensor(inertiaTensor);

    bodies[id] = newBody;
    return newBody;
}

Collider* Simulator::addCollider(unsigned int id, Geometry geometry, RigidBody* body)
{
    Collider* newCollider;

    if (geometry == SPHERE)
        newCollider = new SphereCollider(body, 1.0f);
    else if (geometry == BOX)
        newCollider = new BoxCollider(body, 0.5f, 0.5f, 0.5f);
    
    colliders[id] = newCollider;
    return newCollider;
}

void Simulator::removePhysicsObject(unsigned int id)
{
    RigidBodies::iterator bodyIter = bodies.find(id);
    Colliders::iterator colliderIter = colliders.find(id);

    if (bodyIter != bodies.end())
    {
        delete bodyIter->second;
        bodies.erase(bodyIter);
    }
    else
        std::cout << "ERROR::Simulator::removePhysicsObject()::can't find RigidBody id: " << id << std::endl;
    
    if (colliderIter != colliders.end())
    {
        delete colliderIter->second;
        colliders.erase(colliderIter);
    }
    else
        std::cout << "ERROR::Simulator::removePhysicsObject()::can't find Collider id: " << id << std::endl;
}

float Simulator::calcDistanceBetweenRayAndObject(
    const Vector3& rayOrigin,
    const Vector3& rayDirection,
    const unsigned int id
)
{
    float distance;
    Collider* collider = colliders.find(id)->second;

    if (typeid(*collider) == typeid(SphereCollider))
    {
        SphereCollider* sphere = static_cast<SphereCollider*>(collider);
        distance = detector.rayAndSphere(rayOrigin, rayDirection, *sphere);
    }
    else if (typeid(*collider) == typeid(BoxCollider))
    {
        BoxCollider* box = static_cast<BoxCollider*>(collider);
        distance = detector.rayAndBox(rayOrigin, rayDirection, *box);
    }

    return distance;
}

void Simulator::getContactInfo(std::vector<ContactInfo*>& contactInfo) const
{
    for (const auto& contact : contacts)
    {
        for (const auto& cp : contact->contactPoint)
        {
            if (cp == nullptr)
                continue;
                
            ContactInfo* newContactInfo = new ContactInfo;
            newContactInfo->pointX = cp->x;
            newContactInfo->pointY = cp->y;
            newContactInfo->pointZ = cp->z;
            newContactInfo->normalX = contact->normal.x;
            newContactInfo->normalY = contact->normal.y;
            newContactInfo->normalZ = contact->normal.z;
            contactInfo.push_back(newContactInfo);
        }
    }
}

void Simulator::setGroundRestitution(float value)
{
    detector.groundRestitution = value;
}

void Simulator::setObjectRestitution(float value)
{
    detector.objectRestitution = value;
}

void Simulator::setGravity(float value)
{
    gravity = value;
    for (auto& body : bodies)
        body.second->setAcceleration(0.0f, -gravity, 0.0f);
}
