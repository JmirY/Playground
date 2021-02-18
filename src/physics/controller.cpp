#include <physics/controller.h>
#include <typeinfo>

using namespace physics;

PhysicsController::~PhysicsController()
{
    /* 충돌 정보 해제 */
    Contacts::iterator i = contacts.begin();
    for (; i != contacts.end(); ++i)
        delete *i;
    
    /* 충돌체 해제 */
    Colliders::iterator j = colliders.begin();
    for (; j != colliders.end(); ++j)
        delete *j;
    
    /* 강체 해제 */
    RigidBodies::iterator k = bodies.begin();
    for (; k != bodies.end(); ++k)
        delete *k;
}

void PhysicsController::simulate(float duration)
{
    /* 물체들을 적분한다 */
    for (RigidBodies::iterator i = bodies.begin(); i != bodies.end(); ++i)
    {
        (*i)->integrate(duration);
    }

    /* 물체 간 충돌을 검출한다 */
    detectCollision();

    /* 충돌들을 처리한다 */
    for (Contacts::iterator i = contacts.begin(); i != contacts.end(); ++i)
    {
        (*i)->resolve();
        delete *i;
    }
    contacts.clear();
}

void PhysicsController::addPhysicsObject(Geometry geometry)
{
    /* 강체를 생성한다 */
    RigidBody* newBody = new RigidBody;
    newBody->setMass(5.0f);
    newBody->setPosition(0.0f, 2.0f, 0.0f);
    newBody->setAcceleration(0.0f, -1.0f, 0.0f);

    /* 강체의 관성 모멘트 텐서와 충돌체를 생성한다 */
    Matrix3 inertiaTensor;
    Collider* newCollider;
    if (geometry == SPHERE)
    {
        inertiaTensor.setDiagonal(2.0f);
        newCollider = new SphereCollider(newBody, 1.0f);
    }
    else if (geometry == BOX)
    {
        inertiaTensor.setDiagonal(2.0f);
        newCollider = new BoxCollider(newBody, 0.5f, 0.5f, 0.5f);
    }
    newBody->setInertiaTensor(inertiaTensor);

    bodies.push_back(newBody);
    colliders.push_back(newCollider);
}

void PhysicsController::detectCollision()
{
    for (Colliders::iterator i = colliders.begin(); i != colliders.end(); ++i)
    {
        for (Colliders::iterator j = i + 1; j != colliders.end(); ++j)
        {
            if ((*i)->geometry == SPHERE)
            {
                SphereCollider* collider1 = static_cast<SphereCollider*>(*i);
                if ((*j)->geometry == SPHERE) // 구 - 구 충돌
                {
                    SphereCollider* collider2 = static_cast<SphereCollider*>(*j);
                    CollisionDetector::sphereAndSphere(contacts, *collider1, *collider2);
                }
                else if ((*j)->geometry == BOX) // 구 - 직육면체 충돌
                {
                    BoxCollider* collider2 = static_cast<BoxCollider*>(*j);
                    CollisionDetector::sphereAndBox(contacts, *collider1, *collider2);
                }
            }
            else if ((*i)->geometry == BOX)
            {
                BoxCollider* collider1 = static_cast<BoxCollider*>(*i);
                if ((*j)->geometry == SPHERE) // 구 - 직육면체 충돌
                {
                    SphereCollider* collider2 = static_cast<SphereCollider*>(*j);
                    CollisionDetector::sphereAndBox(contacts, *collider2, *collider1);
                }
                else if ((*j)->geometry == BOX) // 직육면체 - 직육면체 충돌
                {
                    BoxCollider* collider2 = static_cast<BoxCollider*>(*j);
                    CollisionDetector::boxAndBox(contacts, *collider1, *collider2);
                }
            }
        }

        /* 지면과의 충돌 검사 */
        if ((*i)->geometry == SPHERE)
        {
            SphereCollider* sphereCollider = static_cast<SphereCollider*>(*i);
            CollisionDetector::sphereAndPlane(contacts, *sphereCollider, groundCollider);
        }
        else if ((*i)->geometry == BOX)
        {
            BoxCollider* boxCollider = static_cast<BoxCollider*>(*i);
            CollisionDetector::boxAndPlane(contacts, *boxCollider, groundCollider);
        }
    }
}
