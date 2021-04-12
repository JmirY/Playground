#include <physics/simulator.h>
#include <iterator>
#include <typeinfo>
#include <cmath>

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
    detectCollision();

    /* 충돌 정보를 복사한다 */
    getContactInfo(contactInfo);

    /* 충돌들을 처리한다 */
    for (auto& contact : contacts)
    {
        resolver.resolveContact(contact);
        delete contact;
    }
    contacts.clear();
}

RigidBody* Simulator::addRigidBody(unsigned int id, Geometry geometry)
{
    /* 강체를 생성한다 */
    RigidBody* newBody = new RigidBody;
    newBody->setMass(5.0f);
    newBody->setPosition(0.0f, 4.0f, 0.0f);
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

    delete bodyIter->second;
    delete colliderIter->second;

    bodies.erase(bodyIter);
    colliders.erase(colliderIter);
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
        ContactInfo* newContactInfo = new ContactInfo;

        newContactInfo->pointX = contact->contactPoint.x;
        newContactInfo->pointY = contact->contactPoint.y;
        newContactInfo->pointZ = contact->contactPoint.z;
        newContactInfo->normalX = contact->normal.x;
        newContactInfo->normalY = contact->normal.y;
        newContactInfo->normalZ = contact->normal.z;

        contactInfo.push_back(newContactInfo);
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

void Simulator::detectCollision()
{
    for (Colliders::iterator i = colliders.begin(); i != colliders.end(); ++i)
    {
        Collider* colliderPtrI = i->second;
        for (Colliders::iterator j = std::next(i, 1); j != colliders.end(); ++j)
        {
            Collider* colliderPtrJ = j->second;
            if (typeid(*colliderPtrI) == typeid(SphereCollider))
            {
                SphereCollider* collider1 = static_cast<SphereCollider*>(colliderPtrI);
                if (typeid(*colliderPtrJ) == typeid(SphereCollider)) // 구 - 구 충돌
                {
                    SphereCollider* collider2 = static_cast<SphereCollider*>(colliderPtrJ);
                    detector.sphereAndSphere(contacts, *collider1, *collider2);
                }
                else if (typeid(*colliderPtrJ) == typeid(BoxCollider)) // 구 - 직육면체 충돌
                {
                    BoxCollider* collider2 = static_cast<BoxCollider*>(colliderPtrJ);
                    detector.sphereAndBox(contacts, *collider1, *collider2);
                }
            }
            else if (typeid(*colliderPtrI) == typeid(BoxCollider))
            {
                BoxCollider* collider1 = static_cast<BoxCollider*>(colliderPtrI);
                if (typeid(*colliderPtrJ) == typeid(SphereCollider)) // 구 - 직육면체 충돌
                {
                    SphereCollider* collider2 = static_cast<SphereCollider*>(colliderPtrJ);
                    detector.sphereAndBox(contacts, *collider2, *collider1);
                }
                else if (typeid(*colliderPtrJ) == typeid(BoxCollider)) // 직육면체 - 직육면체 충돌
                {
                    BoxCollider* collider2 = static_cast<BoxCollider*>(colliderPtrJ);
                    detector.boxAndBox(contacts, *collider1, *collider2);
                }
            }
        }

        /* 지면과의 충돌 검사 */
        if (typeid(*colliderPtrI) == typeid(SphereCollider))
        {
            SphereCollider* sphereCollider = static_cast<SphereCollider*>(i->second);
            detector.sphereAndPlane(contacts, *sphereCollider, groundCollider);
        }
        else if (typeid(*colliderPtrI) == typeid(BoxCollider))
        {
            BoxCollider* boxCollider = static_cast<BoxCollider*>(i->second);
            detector.boxAndPlane(contacts, *boxCollider, groundCollider);
        }
    }
}
