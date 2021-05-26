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

    Vector3 contactPointFromCenter1 = *contact->contactPoint[0] - contact->bodies[0]->getPosition();
    Vector3 contactPointFromCenter2;
    if (contact->bodies[1] != nullptr)
        contactPointFromCenter2 = *contact->contactPoint[1] - contact->bodies[1]->getPosition();

    Vector3 termInDenominator1 = (contact->bodies[0]->getInverseInertiaTensorWorld() * (contactPointFromCenter1.cross(contact->normal)))
        .cross(contactPointFromCenter1);
    Vector3 termInDenominator2;
    if (contact->bodies[1] != nullptr) {
        termInDenominator2 =
            (contact->bodies[1]->getInverseInertiaTensorWorld() * (contactPointFromCenter2.cross(contact->normal)))
                .cross(contactPointFromCenter2);
    }
    effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).dot(contact->normal);
    if (effectiveMass == 0.0f)
        return;

    float closingSpeed = contact->normal.dot(contact->bodies[0]->getVelocity())
        + contact->bodies[0]->getRotation().dot(contactPointFromCenter1.cross(contact->normal));
    if (contact->bodies[1] != nullptr)
    {
        closingSpeed -= contact->normal.dot(contact->bodies[1]->getVelocity())
            + contact->bodies[1]->getRotation().dot(contactPointFromCenter2.cross(contact->normal));
    }

    /* bias 계산 */
    float bias = 0.0f;
    /* Baumgarte Stabilization */
    float baumgarte = 0.0f;
    if (contact->penetration > penetrationTolerance)
        baumgarte = (-0.1f / deltaTime) * (contact->penetration - penetrationTolerance);
    bias += baumgarte;

    /* bias 에 restitution term 추가 */
    float restitutionTerm = 0.0f;
    if (closingSpeed > closingSpeedTolerance)
        restitutionTerm = contact->restitution * (closingSpeed - closingSpeedTolerance);
    bias -= restitutionTerm;

    float impulse = -(closingSpeed + bias) / effectiveMass;
    if (isnan(impulse) != 0)
    {
        std::cout << "ERROR::CollisionResolver::sequentialImpulse()::impulse is nan" << std::endl;
        return;
    }

    /* 충격량의 누적값을 clamp */
    float prevImpulseSum = contact->normalImpulseSum;
    contact->normalImpulseSum += impulse;
    if (contact->normalImpulseSum < 0.0f)
        contact->normalImpulseSum = 0.0f;
    impulse = contact->normalImpulseSum - prevImpulseSum;

    /* 속도 & 각속도 변화 */
    Vector3 linearImpulse = contact->normal * impulse;
    Vector3 angularImpulse1 = contactPointFromCenter1.cross(contact->normal) * impulse;
    Vector3 angularImpulse2 = contactPointFromCenter2.cross(contact->normal) * impulse;

    contact->bodies[0]->setVelocity(
        contact->bodies[0]->getVelocity() + linearImpulse * contact->bodies[0]->getInverseMass()
    );
    contact->bodies[0]->setRotation(
        contact->bodies[0]->getRotation() + contact->bodies[0]->getInverseInertiaTensorWorld() * angularImpulse1
    );
    if (contact->bodies[1] != nullptr)
    {
        contact->bodies[1]->setVelocity(
            contact->bodies[1]->getVelocity() - linearImpulse * contact->bodies[1]->getInverseMass()
        );
        contact->bodies[1]->setRotation(
            contact->bodies[1]->getRotation() - contact->bodies[1]->getInverseInertiaTensorWorld() * angularImpulse2
        );
    }

    /* 충돌 법선에 수직하는 벡터 찾기(erin catto 방법) */
    Vector3 tangent1, tangent2;
    if (contact->normal.x >= 0.57735f)
        tangent1 = Vector3(contact->normal.y, -contact->normal.x, 0.0f);
    else
        tangent1 = Vector3(0.0f, contact->normal.z, -contact->normal.y);
    tangent2 = contact->normal.cross(tangent1);

    /* tangent1 벡터에 대한 마찰 계산 */
    termInDenominator1 = (contact->bodies[0]->getInverseInertiaTensorWorld() * (contactPointFromCenter1.cross(tangent1)))
        .cross(contactPointFromCenter1);
    if (contact->bodies[1] != nullptr) {
        termInDenominator2 =
            (contact->bodies[1]->getInverseInertiaTensorWorld() * (contactPointFromCenter2.cross(tangent1)))
                .cross(contactPointFromCenter2);
    }
    effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).dot(tangent1);

    closingSpeed = tangent1.dot(contact->bodies[0]->getVelocity())
        + contact->bodies[0]->getRotation().dot(contactPointFromCenter1.cross(tangent1));
    if (contact->bodies[1] != nullptr)
    {
        closingSpeed -= tangent1.dot(contact->bodies[1]->getVelocity())
            + contact->bodies[1]->getRotation().dot(contactPointFromCenter2.cross(tangent1));
    }
    impulse = -closingSpeed / effectiveMass;
    if (isnan(impulse) != 0)
    {
        std::cout << "ERROR::CollisionResolver::sequentialImpulse()::tangential impulse1 is nan" << std::endl;
        return;
    }

    /* 람다의 누적값을 clamp */
    prevImpulseSum = contact->tangentImpulseSum1;
    contact->tangentImpulseSum1 += impulse;
    if (contact->tangentImpulseSum1 < (-contact->friction * contact->normalImpulseSum))
        contact->tangentImpulseSum1 = -contact->friction * contact->normalImpulseSum;
    else if (contact->tangentImpulseSum1 > (contact->friction * contact->normalImpulseSum))
        contact->tangentImpulseSum1 = contact->friction * contact->normalImpulseSum;
    impulse = contact->tangentImpulseSum1 - prevImpulseSum;

    /* 속도 & 각속도 변화 */
    linearImpulse = tangent1 * impulse;
    angularImpulse1 = contactPointFromCenter1.cross(tangent1) * impulse;
    angularImpulse2 = contactPointFromCenter2.cross(tangent1) * impulse;

    contact->bodies[0]->setVelocity(
        contact->bodies[0]->getVelocity() + linearImpulse * contact->bodies[0]->getInverseMass()
    );
    contact->bodies[0]->setRotation(
        contact->bodies[0]->getRotation() + contact->bodies[0]->getInverseInertiaTensorWorld() * angularImpulse1
    );
    if (contact->bodies[1] != nullptr)
    {
        contact->bodies[1]->setVelocity(
            contact->bodies[1]->getVelocity() - linearImpulse * contact->bodies[1]->getInverseMass()
        );
        contact->bodies[1]->setRotation(
            contact->bodies[1]->getRotation() - contact->bodies[1]->getInverseInertiaTensorWorld() * angularImpulse2
        );
    }

    /* tangent2 벡터에 대한 마찰 계산 */
    termInDenominator1 = (contact->bodies[0]->getInverseInertiaTensorWorld() * (contactPointFromCenter1.cross(tangent2)))
        .cross(contactPointFromCenter1);
    if (contact->bodies[1] != nullptr) {
        termInDenominator2 =
            (contact->bodies[1]->getInverseInertiaTensorWorld() * (contactPointFromCenter2.cross(tangent2)))
                .cross(contactPointFromCenter2);
    }
    effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).dot(tangent2);

    closingSpeed = tangent2.dot(contact->bodies[0]->getVelocity())
        + contact->bodies[0]->getRotation().dot(contactPointFromCenter1.cross(tangent2));
    if (contact->bodies[1] != nullptr)
    {
        closingSpeed -= tangent2.dot(contact->bodies[1]->getVelocity())
            + contact->bodies[1]->getRotation().dot(contactPointFromCenter2.cross(tangent2));
    }
    impulse = -closingSpeed / effectiveMass;
    if (isnan(impulse) != 0)
    {
        std::cout << "ERROR::CollisionResolver::sequentialImpulse()::tangential impulse2 is nan" << std::endl;
        return;
    }

    /* 람다의 누적값을 clamp */
    prevImpulseSum = contact->tangentImpulseSum2;
    contact->tangentImpulseSum2 += impulse;
    if (contact->tangentImpulseSum2 < (-contact->friction * contact->normalImpulseSum))
        contact->tangentImpulseSum2 = -contact->friction * contact->normalImpulseSum;
    else if (contact->tangentImpulseSum2 > (contact->friction * contact->normalImpulseSum))
        contact->tangentImpulseSum2 = contact->friction * contact->normalImpulseSum;
    impulse = contact->tangentImpulseSum2 - prevImpulseSum;

    /* 속도 & 각속도 변화 */
    linearImpulse = tangent2 * impulse;
    angularImpulse1 = contactPointFromCenter1.cross(tangent2) * impulse;
    angularImpulse2 = contactPointFromCenter2.cross(tangent2) * impulse;

    contact->bodies[0]->setVelocity(
        contact->bodies[0]->getVelocity() + linearImpulse * contact->bodies[0]->getInverseMass()
    );
    contact->bodies[0]->setRotation(
        contact->bodies[0]->getRotation() + contact->bodies[0]->getInverseInertiaTensorWorld() * angularImpulse1
    );
    if (contact->bodies[1] != nullptr)
    {
        contact->bodies[1]->setVelocity(
            contact->bodies[1]->getVelocity() - linearImpulse * contact->bodies[1]->getInverseMass()
        );
        contact->bodies[1]->setRotation(
            contact->bodies[1]->getRotation() - contact->bodies[1]->getInverseInertiaTensorWorld() * angularImpulse2
        );
    }
}
