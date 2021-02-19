#include <physics/simulator.h>
#include <iterator>

using namespace physics;

Simulator::~Simulator()
{
    /* 충돌 정보 해제 */
    for (Contact* contact : contacts)
        delete contact;
    
    /* 충돌체 해제 */
    for (auto collider : colliders)
        delete collider.second;
    
    /* 강체 해제 */
    for (auto body : bodies)
        delete body.second;
}

void Simulator::simulate(float duration)
{
    /* 물체들을 적분한다 */
    for (auto body : bodies)
    {
        body.second->integrate(duration);
    }

    /* 물체 간 충돌을 검출한다 */
    detectCollision();

    /* 충돌들을 처리한다 */
    for (Contact* contact : contacts)
    {
        contact->resolve();
        delete contact;
    }
    contacts.clear();
}

RigidBody* Simulator::addRigidBody(unsigned int id, Geometry geometry)
{
    /* 강체를 생성한다 */
    RigidBody* newBody = new RigidBody;
    newBody->setMass(5.0f);
    newBody->setPosition(0.0f, 2.0f, 0.0f);
    newBody->setAcceleration(0.0f, -1.0f, 0.0f);

    /* 강체의 관성 모멘트 텐서를 도형에 따라 결정한다 */
    Matrix3 inertiaTensor;
    if (geometry == SPHERE)
    {
        inertiaTensor.setDiagonal(2.0f);
    }
    else if (geometry == BOX)
    {
        inertiaTensor.setDiagonal(0.208333f);
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

    /* 벡터에서 해당 주소값들을 제거한다 */
    bodies.erase(bodyIter);
    colliders.erase(colliderIter);
    
    /* 메모리를 해제한다 */
    delete bodyIter->second;
    delete colliderIter->second;
}

void Simulator::detectCollision()
{
    for (Colliders::iterator i = colliders.begin(); i != colliders.end(); ++i)
    {
        for (Colliders::iterator j = std::next(i, 1); j != colliders.end(); ++j)
        {
            if (i->second->geometry == SPHERE)
            {
                SphereCollider* collider1 = static_cast<SphereCollider*>(i->second);
                if (j->second->geometry == SPHERE) // 구 - 구 충돌
                {
                    SphereCollider* collider2 = static_cast<SphereCollider*>(j->second);
                    CollisionDetector::sphereAndSphere(contacts, *collider1, *collider2);
                }
                else if (j->second->geometry == BOX) // 구 - 직육면체 충돌
                {
                    BoxCollider* collider2 = static_cast<BoxCollider*>(j->second);
                    CollisionDetector::sphereAndBox(contacts, *collider1, *collider2);
                }
            }
            else if (i->second->geometry == BOX)
            {
                BoxCollider* collider1 = static_cast<BoxCollider*>(i->second);
                if (j->second->geometry == SPHERE) // 구 - 직육면체 충돌
                {
                    SphereCollider* collider2 = static_cast<SphereCollider*>(j->second);
                    CollisionDetector::sphereAndBox(contacts, *collider2, *collider1);
                }
                else if (j->second->geometry == BOX) // 직육면체 - 직육면체 충돌
                {
                    BoxCollider* collider2 = static_cast<BoxCollider*>(j->second);
                    CollisionDetector::boxAndBox(contacts, *collider1, *collider2);
                }
            }
        }

        /* 지면과의 충돌 검사 */
        if (i->second->geometry == SPHERE)
        {
            SphereCollider* sphereCollider = static_cast<SphereCollider*>(i->second);
            CollisionDetector::sphereAndPlane(contacts, *sphereCollider, groundCollider);
        }
        else if (i->second->geometry == BOX)
        {
            BoxCollider* boxCollider = static_cast<BoxCollider*>(i->second);
            CollisionDetector::boxAndPlane(contacts, *boxCollider, groundCollider);
        }
    }
}
