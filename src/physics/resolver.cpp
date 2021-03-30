#include <physics/resolver.h>
#include <cmath>
#include <iostream>

using namespace physics;

void CollisionResolver::resolveContact(Contact* contact)
{
    resolveVelocity(contact);
    resolvePenetration(contact);
}

void CollisionResolver::resolveVelocity(Contact* contact)
{
    Vector3 contactPointFromCenter1 = contact->contactPoint - contact->bodies[0]->getPosition();
    Vector3 contactPointFromCenter2;
    if (contact->bodies[1] != nullptr)
        contactPointFromCenter2 = contact->contactPoint - contact->bodies[1]->getPosition();

    /* 두 물체의 충돌점의 속도를 계산한다 */
    Vector3 contactPointVelocity1 = contact->bodies[0]->getVelocity() +
        contact->bodies[0]->getRotation().cross(contactPointFromCenter1);

    Vector3 contactPointVelocity2;
    if (contact->bodies[1] != nullptr)
    {
        contactPointVelocity2 = contact->bodies[1]->getVelocity() +
            contact->bodies[1]->getRotation().cross(contactPointFromCenter2);
    }
        
    /* 상대 속도를 계산한다 */
    Vector3 relativeVelocity = contactPointVelocity2 - contactPointVelocity1;

    /* 두 물체의 역질량의 합을 계산한다 */
    float totalInverseMass = contact->bodies[0]->getInverseMass();
    if (contact->bodies[1] != nullptr)
        totalInverseMass += contact->bodies[1]->getInverseMass();
    
    /* 역질량의 합이 0 이라면
        두 물체의 질량이 모두 무한대이므로 함수를 종료한다 */
    if (totalInverseMass == 0.0f)
        return;
    
    /* 충격량의 크기를 계산한다 */
    Vector3 contactNormal = contact->normal * -1.0f;
    float numerator = (relativeVelocity * (1.0f + contact->restitution)).dot(contactNormal) * -1.0f;

    Vector3 termInDenominator1 = (contact->bodies[0]->getInverseInertiaTensorWorld() * (contactPointFromCenter1.cross(contactNormal)))
        .cross(contactPointFromCenter1);
    Vector3 termInDenominator2;
    if (contact->bodies[1] != nullptr) {
        termInDenominator2 =
            (contact->bodies[1]->getInverseInertiaTensorWorld() * (contactPointFromCenter2.cross(contactNormal)))
                .cross(contactPointFromCenter2);
    }
    float denominator = totalInverseMass + (termInDenominator1 + termInDenominator2).dot(contactNormal);
    if (denominator == 0.0f)
        return;

    float impulse = numerator / denominator;
    if (impulse == 0.0f)
        return;
    Vector3 impulseVector = contactNormal * impulse;

    /* 두 물체의 선속도를 갱신한다 */
    contact->bodies[0]->setVelocity(
        contact->bodies[0]->getVelocity() - impulseVector * contact->bodies[0]->getInverseMass()
    );

    if (contact->bodies[1] != nullptr)
    {
        contact->bodies[1]->setVelocity(
            contact->bodies[1]->getVelocity() + impulseVector * contact->bodies[1]->getInverseMass()
        );
    }

    /* 두 물체의 각속도를 갱신한다 */
    Vector3 contactPointFromCenter = contact->contactPoint - contact->bodies[0]->getPosition();
    contact->bodies[0]->setRotation(
        contact->bodies[0]->getRotation() - contact->bodies[0]->getInverseInertiaTensorWorld() * impulse
            * (contactPointFromCenter.cross(contactNormal))
    );

    if (contact->bodies[1] != nullptr)
    {
        contactPointFromCenter = contact->contactPoint - contact->bodies[1]->getPosition();
        contact->bodies[1]->setRotation(
            contact->bodies[1]->getRotation() + contact->bodies[1]->getInverseInertiaTensorWorld() * impulse
                * (contactPointFromCenter.cross(contactNormal))
        );
    }

    /* 마찰을 계산한다 */

    /* 접선 벡터를 계산한다 */
    Vector3 tangentVector = relativeVelocity - contactNormal * (relativeVelocity.dot(contactNormal));
    if (tangentVector.magnitudeSquared() == 0.0f)
        return;
    tangentVector.normalize();

    /* 마찰 임펄스를 계산한다 */
    numerator = relativeVelocity.dot(tangentVector) * -1.0f;

    termInDenominator1 = (contact->bodies[0]->getInverseInertiaTensorWorld() * (contactPointFromCenter1.cross(tangentVector)))
        .cross(contactPointFromCenter1);
    if (contact->bodies[1] != nullptr) {
        termInDenominator2 =
            (contact->bodies[1]->getInverseInertiaTensorWorld() * (contactPointFromCenter2.cross(tangentVector)))
                .cross(contactPointFromCenter2);
    }
    denominator = totalInverseMass + (termInDenominator1 + termInDenominator2).dot(tangentVector);
    if (denominator == 0.0f)
        return;

    float frictionalImpulse = numerator / denominator;
    if (frictionalImpulse == 0.0f)
        return;
    
    if (frictionalImpulse > impulse * contact->friction)
        frictionalImpulse = impulse * contact->friction;
    else if (frictionalImpulse < -1.0f * impulse * contact->friction)
        frictionalImpulse = -1.0f * impulse * contact->friction;
    Vector3 frictionalImpulseVector = tangentVector * frictionalImpulse;

    /* 두 물체의 선속도를 갱신한다 */
    contact->bodies[0]->setVelocity(
        contact->bodies[0]->getVelocity() - frictionalImpulseVector * contact->bodies[0]->getInverseMass()
    );

    if (contact->bodies[1] != nullptr)
    {
        contact->bodies[1]->setVelocity(
            contact->bodies[1]->getVelocity() + frictionalImpulseVector * contact->bodies[1]->getInverseMass()
        );
    }

    /* 두 물체의 각속도를 갱신한다 */
    contactPointFromCenter = contact->contactPoint - contact->bodies[0]->getPosition();
    contact->bodies[0]->setRotation(
        contact->bodies[0]->getRotation() - contact->bodies[0]->getInverseInertiaTensorWorld()
            * (contactPointFromCenter.cross(frictionalImpulseVector))
    );

    if (contact->bodies[1] != nullptr)
    {
        contactPointFromCenter = contact->contactPoint - contact->bodies[1]->getPosition();
        contact->bodies[1]->setRotation(
            contact->bodies[1]->getRotation() + contact->bodies[1]->getInverseInertiaTensorWorld()
                * (contactPointFromCenter.cross(frictionalImpulseVector))
        );
    }
}

// void CollisionResolver::resolveVelocity(Contact* contact)
// {
//     /* 월드 좌표계 -> 충돌 좌표계 변환 행렬을 계산한다 */
//     Matrix3 worldToContact = calcWorldToContactMat(contact->normal);
//     /* 유닛 충격량에 따른 충돌점의 속도 변화를 계산한다 */
//     float deltaVelocityPerUnitImpulse = calcDeltaVelocityPerUnitImpulse(
//         contact->bodies,
//         contact->normal,
//         contact->contactPoint,
//         worldToContact
//     );
//     /* 반발 계수를 이용하여 충돌점의 속도 변화를 계산한다 */
//     float deltaVelocity = calcDeltaVelocity(
//         contact->bodies,
//         contact->contactPoint,
//         contact->restitution,
//         worldToContact
//     );
//     /* 충격량 벡터 (충돌 좌표계 기준) 를 계산한다 */
//     Vector3 impulseVector;
//     impulseVector.x = deltaVelocity / deltaVelocityPerUnitImpulse;
//     /* 월드 좌표계로 변환한다 */
//     impulseVector = worldToContact.transpose() * impulseVector;

//     /* 속도 변화를 적용한다 */
//     Vector3 velocityChange = impulseVector * contact->bodies[0]->getInverseMass();
//     Vector3 relativeContactPoint =
//         contact->contactPoint - contact->bodies[0]->getPosition();
//     Vector3 angularImpulse = relativeContactPoint.cross(impulseVector);
//     Vector3 rotationChange = contact->bodies[0]->getInverseInertiaTensorWorld() * angularImpulse;

//     contact->bodies[0]->setVelocity(
//         contact->bodies[0]->getVelocity() + velocityChange
//     );
//     contact->bodies[0]->setRotation(
//         contact->bodies[0]->getRotation() + rotationChange
//     );

//     if (contact->bodies[1] != nullptr)
//     {
//         impulseVector *= -1.0f;

//         velocityChange = impulseVector * contact->bodies[1]->getInverseMass();
//         relativeContactPoint =
//             contact->contactPoint - contact->bodies[1]->getPosition();
//         angularImpulse = relativeContactPoint.cross(impulseVector);
//         rotationChange = contact->bodies[1]->getInverseInertiaTensorWorld() * angularImpulse;

//         contact->bodies[1]->setVelocity(
//             contact->bodies[1]->getVelocity() + velocityChange
//         );
//         contact->bodies[1]->setRotation(
//             contact->bodies[1]->getRotation() + rotationChange
//         );
//     }
// }

void CollisionResolver::resolvePenetration(Contact* contact)
{
    /* 두 물체의 역질량의 합을 계산한다 */
    float totalInverseMass = contact->bodies[0]->getInverseMass();
    if (contact->bodies[1] != nullptr)
        totalInverseMass += contact->bodies[1]->getInverseMass();

    /* 역질량의 합이 0 이면
        두 물체가 무한의 질량을 가지는 것이므로 함수를 종료한다 */
    if (totalInverseMass == 0.0f)
        return;

    float penetrationPerInverseMass = contact->penetration / totalInverseMass;

    /* 두 물체의 위치를 질량에 반비례하여 조정한다 */
    Vector3 deltaPosition = contact->normal * penetrationPerInverseMass;
    contact->bodies[0]->setPosition(
        contact->bodies[0]->getPosition() + deltaPosition * contact->bodies[0]->getInverseMass()
    );
    if (contact->bodies[1] != nullptr)
    {
        contact->bodies[1]->setPosition(
            contact->bodies[1]->getPosition() - deltaPosition * contact->bodies[1]->getInverseMass()
        );
    }
}

Matrix3 CollisionResolver::calcWorldToContactMat(const Vector3& contactNormal)
{
    Matrix3 contactToWorld;
    /* 충돌 좌표계의 x 축은 법선과 같다 */
    contactToWorld.entries[0] = contactNormal.x;
    contactToWorld.entries[3] = contactNormal.y;
    contactToWorld.entries[6] = contactNormal.z;

    Vector3 yAxis, zAxis;
    if (fabs(contactNormal.x) > fabs(contactNormal.y))  // 법선이 월드 좌표계 x 축에 더 가까울 때
        yAxis.y = 1.0f;
    else                                                // 법선이 월드 좌표계 y 축에 더 가까울 때
        yAxis.x = 1.0f;
    zAxis = contactNormal.cross(yAxis);
    zAxis.normalize();
    yAxis = zAxis.cross(contactNormal);

    contactToWorld.entries[1] = yAxis.x;
    contactToWorld.entries[4] = yAxis.y;
    contactToWorld.entries[7] = yAxis.z;
    contactToWorld.entries[2] = zAxis.x;
    contactToWorld.entries[5] = zAxis.y;
    contactToWorld.entries[8] = zAxis.z;

    return contactToWorld.transpose();
}

float CollisionResolver::calcDeltaVelocityPerUnitImpulse(
    RigidBody* (&bodies)[2],
    const Vector3& contactNormal,
    const Vector3& contactPoint,
    const Matrix3& worldToContact
)
{
    /* 첫 번째 강체의 선속도 성분 더하기 */
    float deltaVelocityPerUnitImpulse = bodies[0]->getInverseMass();
    /* 첫 번째 강체의 각속도 성분 더하기 */
    Vector3 relativeContactPoint = contactPoint - bodies[0]->getPosition();
    Vector3 angularImpulsePerUnitLinearImpulse = 
        relativeContactPoint.cross(contactNormal);
    Vector3 deltaRotationPerUnitImpulse = 
        bodies[0]->getInverseInertiaTensorWorld() * angularImpulsePerUnitLinearImpulse;
    deltaRotationPerUnitImpulse = deltaRotationPerUnitImpulse.cross(relativeContactPoint);
    deltaRotationPerUnitImpulse = worldToContact * deltaRotationPerUnitImpulse;
    deltaVelocityPerUnitImpulse += deltaRotationPerUnitImpulse.x;

    if (bodies[1] != nullptr)
    {
        /* 두 번째 강체의 선속도 성분 더하기 */
        deltaVelocityPerUnitImpulse += bodies[1]->getInverseMass();
        /* 두 번째 강체의 각속도 성분 더하기 */
        relativeContactPoint = contactPoint - bodies[1]->getPosition();
        angularImpulsePerUnitLinearImpulse = 
            relativeContactPoint.cross(contactNormal);
        deltaRotationPerUnitImpulse = 
            bodies[1]->getInverseInertiaTensorWorld() * angularImpulsePerUnitLinearImpulse;
        deltaRotationPerUnitImpulse = deltaRotationPerUnitImpulse.cross(relativeContactPoint);
        deltaRotationPerUnitImpulse = worldToContact * deltaRotationPerUnitImpulse;
        deltaVelocityPerUnitImpulse += deltaRotationPerUnitImpulse.x;
    }

    return deltaVelocityPerUnitImpulse;
}

float CollisionResolver::calcDeltaVelocity(
    RigidBody* (&bodies)[2],
    const Vector3& contactPoint,
    const float restitution,
    const Matrix3& worldToContact
)
{
    Vector3 currentVelocity = bodies[0]->getVelocity();
    Vector3 relativeContactPoint = contactPoint - bodies[0]->getPosition();
    currentVelocity += bodies[0]->getRotation().cross(relativeContactPoint);

    Vector3 currentVelocity2;
    if (bodies[1] != nullptr)
    {
        currentVelocity2 += bodies[1]->getVelocity();
        relativeContactPoint = contactPoint - bodies[1]->getPosition();
        currentVelocity2 += bodies[1]->getRotation().cross(relativeContactPoint);
    }
    currentVelocity -= currentVelocity2;

    Vector3 currentVelocityInContact = worldToContact * currentVelocity;

    float deltaVelocity = -currentVelocityInContact.x * (1 + restitution);
    return deltaVelocity;
}
