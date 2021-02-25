#include <physics/contact.h>

using namespace physics;

void Contact::resolveVelocity()
{
    Vector3 contactPointFromCenter1 = contactPoint - bodies[0]->getPosition();
    Vector3 contactPointFromCenter2;
    if (bodies[1] != nullptr)
        contactPointFromCenter2 = contactPoint - bodies[1]->getPosition();

    /* 두 물체의 충돌점의 속도를 계산한다 */
    Vector3 contactPointVelocity1 = bodies[0]->getVelocity() +
        bodies[0]->getRotation().cross(contactPointFromCenter1);

    Vector3 contactPointVelocity2;
    if (bodies[1] != nullptr)
    {
        contactPointVelocity2 = bodies[1]->getVelocity() +
            bodies[1]->getRotation().cross(contactPointFromCenter2);
    }
        
    /* 상대 속도를 계산한다 */
    Vector3 relativeVelocity = contactPointVelocity2 - contactPointVelocity1;

    /* 두 물체의 역질량의 합을 계산한다 */
    float totalInverseMass = bodies[0]->getInverseMass();
    if (bodies[1] != nullptr)
        totalInverseMass += bodies[1]->getInverseMass();
    
    /* 역질량의 합이 0 이라면
        두 물체의 질량이 모두 무한대이므로 함수를 종료한다 */
    if (totalInverseMass == 0.0f)
        return;
    
    /* 충격량의 크기를 계산한다 */
    float numerator = (relativeVelocity * (1.0f + restitution)).dot(normal);

    Vector3 termInDenominator1 =
        (bodies[0]->getInverseInertiaTensorWorld() * (contactPointFromCenter1.cross(normal * -1.0f))).cross(contactPointFromCenter1);
    Vector3 termInDenominator2;
    if (bodies[1] != nullptr) {
        termInDenominator2 =
            (bodies[1]->getInverseInertiaTensorWorld() * (contactPointFromCenter2.cross(normal * -1.0f))).cross(contactPointFromCenter2);
    }
    float denominator = totalInverseMass +
        (termInDenominator1 + termInDenominator2).dot(normal * -1.0f);

    float impulse = numerator / denominator;
    Vector3 impulseVector = normal * impulse;

    /* 두 물체의 선속도를 갱신한다 */
    bodies[0]->setVelocity(
        bodies[0]->getVelocity() + impulseVector * bodies[0]->getInverseMass()
    );

    if (bodies[1] != nullptr)
    {
        bodies[1]->setVelocity(
            bodies[1]->getVelocity() - impulseVector * bodies[1]->getInverseMass()
        );
    }

    /* 두 물체의 각속도를 갱신한다 */
    Vector3 contactPointFromCenter = contactPoint - bodies[0]->getPosition();
    bodies[0]->setRotation(
        bodies[0]->getRotation() + bodies[0]->getInverseInertiaTensorWorld() * impulse * (contactPointFromCenter.cross(normal))
    );

    if (bodies[1] != nullptr)
    {
        contactPointFromCenter = contactPoint - bodies[1]->getPosition();
        bodies[1]->setRotation(
            bodies[1]->getRotation() - bodies[1]->getInverseInertiaTensorWorld() * impulse * (contactPointFromCenter.cross(normal))
        );
    }
}

/* linear projection */
void Contact::resolvePenetration()
{
    /* 두 물체의 역질량의 합을 계산한다 */
    float totalInverseMass = bodies[0]->getInverseMass();
    if (bodies[1] != nullptr)
        totalInverseMass += bodies[1]->getInverseMass();

    /* 역질량의 합이 0 이면
        두 물체가 무한의 질량을 가지는 것이므로 함수를 종료한다 */
    if (totalInverseMass == 0.0f)
        return;

    float penetrationPerInverseMass = penetration / totalInverseMass;

    /* 두 물체의 위치를 질량에 반비례하여 조정한다 */
    Vector3 deltaPosition = normal * penetrationPerInverseMass;
    bodies[0]->setPosition(
        bodies[0]->getPosition() + deltaPosition * bodies[0]->getInverseMass()
    );
    if (bodies[1] != nullptr)
    {
        bodies[1]->setPosition(
            bodies[1]->getPosition() - deltaPosition * bodies[1]->getInverseMass()
        );
    }
}

