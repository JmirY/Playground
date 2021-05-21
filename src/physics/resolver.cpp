#include <physics/resolver.h>
#include <cmath>
#include <iostream>

using namespace physics;

void CollisionResolver::resolveCollision(std::vector<Contact*>& contacts, float deltaTime)
{
    for (int i = 0; i < iterationLimit; ++i)
    {
        for (auto& contact : contacts)
        {
            sequentialImpulse(contact, deltaTime);
        }
    }
}

void CollisionResolver::sequentialImpulse(Contact* contact, float deltaTime)
{
    float effectiveMass;

    float totalInvMass = contact->bodies[0]->getInverseMass();
    if (contact->bodies[1] != nullptr)
        totalInvMass += contact->bodies[1]->getInverseMass();
    if (totalInvMass == 0.0f)
        return;

    Vector3 contactPointFromCenter1 = contact->contactPoint - contact->bodies[0]->getPosition();
    Vector3 contactPointFromCenter2;
    if (contact->bodies[1] != nullptr)
        contactPointFromCenter2 = contact->contactPoint - contact->bodies[1]->getPosition();

    Vector3 termInDenominator1 = (contact->bodies[0]->getInverseInertiaTensorWorld() * (contactPointFromCenter1.cross(contact->normal)))
        .cross(contactPointFromCenter1);
    Vector3 termInDenominator2;
    if (contact->bodies[1] != nullptr) {
        termInDenominator2 =
            (contact->bodies[1]->getInverseInertiaTensorWorld() * (contactPointFromCenter2.cross(contact->normal)))
                .cross(contactPointFromCenter2);
    }
    effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).dot(contact->normal);

    float lambda = contact->normal.dot(contact->bodies[0]->getVelocity())
        + contact->bodies[0]->getRotation().dot(contactPointFromCenter1.cross(contact->normal));
    if (contact->bodies[1] != nullptr)
    {
        lambda -= contact->normal.dot(contact->bodies[1]->getVelocity())
            + contact->bodies[1]->getRotation().dot(contactPointFromCenter2.cross(contact->normal));
    }

    /* bias 계산 */
    float bias = (-0.1f / deltaTime) * contact->penetration;  // Baumgarte Stabilization
    /* bias 에 restitution term 추가 */
    bias -= contact->restitution * lambda;

    lambda = -(lambda + bias) / effectiveMass;

    /* 람다의 누적값을 clamp */
    float prevNormalImpulseSum = contact->normalImpulseSum;
    contact->normalImpulseSum += lambda;
    if (contact->normalImpulseSum < 0.0f)
        contact->normalImpulseSum *= -1.0f;
    lambda = contact->normalImpulseSum - prevNormalImpulseSum;

    /* 속도 & 각속도 변화 */
    Vector3 deltaVelocity = contact->normal * contact->bodies[0]->getInverseMass() * lambda;
    Vector3 deltaRotation = contact->bodies[0]->getInverseInertiaTensorWorld()
        * (contactPointFromCenter1.cross(contact->normal)) * lambda;
    contact->bodies[0]->setVelocity(
        contact->bodies[0]->getVelocity() + deltaVelocity
    );
    contact->bodies[0]->setRotation(
        contact->bodies[0]->getRotation() + deltaRotation
    );
    if (contact->bodies[1] != nullptr)
    {
        deltaVelocity = contact->normal * -1.0f * contact->bodies[1]->getInverseMass() * lambda;
        deltaRotation = contact->bodies[0]->getInverseInertiaTensorWorld()
            * (contactPointFromCenter1.cross(contact->normal * -1.0f)) * lambda;
        contact->bodies[1]->setVelocity(
            contact->bodies[1]->getVelocity() + deltaVelocity
        );
        contact->bodies[1]->setRotation(
            contact->bodies[1]->getRotation() + deltaRotation
        );
    }
}
