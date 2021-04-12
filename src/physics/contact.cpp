#include <physics/contact.h>

using namespace physics;

// void Contact::resolveVelocity()
// {
//     Vector3 contactPointFromCenter1 = contactPoint - bodies[0]->getPosition();
//     Vector3 contactPointFromCenter2;
//     if (bodies[1] != nullptr)
//         contactPointFromCenter2 = contactPoint - bodies[1]->getPosition();

//     /* 두 물체의 충돌점의 속도를 계산한다 */
//     Vector3 contactPointVelocity1 = bodies[0]->getVelocity() +
//         bodies[0]->getRotation().cross(contactPointFromCenter1);

//     Vector3 contactPointVelocity2;
//     if (bodies[1] != nullptr)
//     {
//         contactPointVelocity2 = bodies[1]->getVelocity() +
//             bodies[1]->getRotation().cross(contactPointFromCenter2);
//     }
        
//     /* 상대 속도를 계산한다 */
//     Vector3 relativeVelocity = contactPointVelocity2 - contactPointVelocity1;

//     /* 두 물체의 역질량의 합을 계산한다 */
//     float totalInverseMass = bodies[0]->getInverseMass();
//     if (bodies[1] != nullptr)
//         totalInverseMass += bodies[1]->getInverseMass();
    
//     /* 역질량의 합이 0 이라면
//         두 물체의 질량이 모두 무한대이므로 함수를 종료한다 */
//     if (totalInverseMass == 0.0f)
//         return;
    
//     /* 충격량의 크기를 계산한다 */
//     Vector3 contactNormal = normal * -1.0f;
//     float numerator = (relativeVelocity * (1.0f + restitution)).dot(contactNormal) * -1.0f;

//     Vector3 termInDenominator1 = (bodies[0]->getInverseInertiaTensorWorld() * (contactPointFromCenter1.cross(contactNormal)))
//         .cross(contactPointFromCenter1);
//     Vector3 termInDenominator2;
//     if (bodies[1] != nullptr) {
//         termInDenominator2 =
//             (bodies[1]->getInverseInertiaTensorWorld() * (contactPointFromCenter2.cross(contactNormal)))
//                 .cross(contactPointFromCenter2);
//     }
//     float denominator = totalInverseMass + (termInDenominator1 + termInDenominator2).dot(contactNormal);
//     if (denominator == 0.0f)
//         return;

//     float impulse = numerator / denominator;
//     if (impulse == 0.0f)
//         return;
//     Vector3 impulseVector = contactNormal * impulse;

//     /* 두 물체의 선속도를 갱신한다 */
//     bodies[0]->setVelocity(
//         bodies[0]->getVelocity() - impulseVector * bodies[0]->getInverseMass()
//     );

//     if (bodies[1] != nullptr)
//     {
//         bodies[1]->setVelocity(
//             bodies[1]->getVelocity() + impulseVector * bodies[1]->getInverseMass()
//         );
//     }

//     /* 두 물체의 각속도를 갱신한다 */
//     Vector3 contactPointFromCenter = contactPoint - bodies[0]->getPosition();
//     bodies[0]->setRotation(
//         bodies[0]->getRotation() - bodies[0]->getInverseInertiaTensorWorld() * impulse
//             * (contactPointFromCenter.cross(contactNormal))
//     );

//     if (bodies[1] != nullptr)
//     {
//         contactPointFromCenter = contactPoint - bodies[1]->getPosition();
//         bodies[1]->setRotation(
//             bodies[1]->getRotation() + bodies[1]->getInverseInertiaTensorWorld() * impulse
//                 * (contactPointFromCenter.cross(contactNormal))
//         );
//     }

//     /* 마찰을 계산한다 */

//     /* 접선 벡터를 계산한다 */
//     Vector3 tangentVector = relativeVelocity - contactNormal * (relativeVelocity.dot(contactNormal));
//     if (tangentVector.magnitudeSquared() == 0.0f)
//         return;
//     tangentVector.normalize();

//     /* 마찰 임펄스를 계산한다 */
//     numerator = relativeVelocity.dot(tangentVector) * -1.0f;

//     termInDenominator1 = (bodies[0]->getInverseInertiaTensorWorld() * (contactPointFromCenter1.cross(tangentVector)))
//         .cross(contactPointFromCenter1);
//     if (bodies[1] != nullptr) {
//         termInDenominator2 =
//             (bodies[1]->getInverseInertiaTensorWorld() * (contactPointFromCenter2.cross(tangentVector)))
//                 .cross(contactPointFromCenter2);
//     }
//     denominator = totalInverseMass + (termInDenominator1 + termInDenominator2).dot(tangentVector);
//     if (denominator == 0.0f)
//         return;

//     float frictionalImpulse = numerator / denominator;
//     if (frictionalImpulse == 0.0f)
//         return;
    
//     if (frictionalImpulse > impulse * friction)
//         frictionalImpulse = impulse * friction;
//     else if (frictionalImpulse < -1.0f * impulse * friction)
//         frictionalImpulse = -1.0f * impulse * friction;
//     Vector3 frictionalImpulseVector = tangentVector * frictionalImpulse;

//     /* 두 물체의 선속도를 갱신한다 */
//     bodies[0]->setVelocity(
//         bodies[0]->getVelocity() - frictionalImpulseVector * bodies[0]->getInverseMass()
//     );

//     if (bodies[1] != nullptr)
//     {
//         bodies[1]->setVelocity(
//             bodies[1]->getVelocity() + frictionalImpulseVector * bodies[1]->getInverseMass()
//         );
//     }

//     /* 두 물체의 각속도를 갱신한다 */
//     contactPointFromCenter = contactPoint - bodies[0]->getPosition();
//     bodies[0]->setRotation(
//         bodies[0]->getRotation() - bodies[0]->getInverseInertiaTensorWorld()
//             * (contactPointFromCenter.cross(frictionalImpulseVector))
//     );

//     if (bodies[1] != nullptr)
//     {
//         contactPointFromCenter = contactPoint - bodies[1]->getPosition();
//         bodies[1]->setRotation(
//             bodies[1]->getRotation() + bodies[1]->getInverseInertiaTensorWorld()
//                 * (contactPointFromCenter.cross(frictionalImpulseVector))
//         );
//     }
// }

// /* linear projection */
// void Contact::resolvePenetration()
// {
//     /* 두 물체의 역질량의 합을 계산한다 */
//     float totalInverseMass = bodies[0]->getInverseMass();
//     if (bodies[1] != nullptr)
//         totalInverseMass += bodies[1]->getInverseMass();

//     /* 역질량의 합이 0 이면
//         두 물체가 무한의 질량을 가지는 것이므로 함수를 종료한다 */
//     if (totalInverseMass == 0.0f)
//         return;

//     float penetrationPerInverseMass = penetration / totalInverseMass;

//     /* 두 물체의 위치를 질량에 반비례하여 조정한다 */
//     Vector3 deltaPosition = normal * penetrationPerInverseMass;
//     bodies[0]->setPosition(
//         bodies[0]->getPosition() + deltaPosition * bodies[0]->getInverseMass()
//     );
//     if (bodies[1] != nullptr)
//     {
//         bodies[1]->setPosition(
//             bodies[1]->getPosition() - deltaPosition * bodies[1]->getInverseMass()
//         );
//     }
// }

/* non-linear projection */
// void Contact::resolvePenetration()
// {
//     float linearInertia[2], angularInertia[2];
//     float linearMove[2], angularMove[2];
//     float totalInertia = 0.0f;
    
//     for (int i = 0; i < 2; ++i)
//     {
//         if (bodies[i] != nullptr)
//         {
//             Matrix3 inverseInertiaTensor = bodies[i]->getInverseInertiaTensorWorld();

//             Vector3 centerToContactPt = contactPoint - bodies[i]->getPosition();
//             Vector3 angularInertialWorld = (centerToContactPt).cross(normal);
//             angularInertialWorld = inverseInertiaTensor * angularInertialWorld;
//             angularInertialWorld = angularInertialWorld.cross(centerToContactPt);
//             angularInertia[i] = angularInertialWorld.dot(normal);

//             linearInertia[i] = bodies[i]->getInverseMass();

//             totalInertia += linearInertia[i] + angularInertia[i];
//         }
//     }

//     float inverseTotalInertia = 1 / totalInertia;
//     linearMove[0] = penetration * linearInertia[0] * inverseTotalInertia;
//     linearMove[1] = -penetration * linearInertia[1] * inverseTotalInertia;
//     angularMove[0] = penetration * angularInertia[1] * inverseTotalInertia;
//     angularMove[1] = -penetration * angularInertia[1] * inverseTotalInertia;

//     for (int i = 0; i < 2; ++i)
//     {
//         if (bodies[i] != nullptr)
//         {
//             /* 선 운동 적용 */
//             Vector3 newPosition = bodies[i]->getPosition() + normal * linearMove[i];
//             bodies[i]->setPosition(newPosition);

//             /* 각 운동 적용 */
//             Vector3 centerToContactPt = contactPoint - bodies[i]->getPosition();
//             Vector3 impulsiveTorque = centerToContactPt.cross(normal);
//             Vector3 impulsePerMove = bodies[i]->getInverseInertiaTensorWorld() * impulsiveTorque;
//             Vector3 rotationPerMove = impulsePerMove * (1 / angularInertia[i]);
//             Vector3 rotation = rotationPerMove * angularMove[i];
//             Quaternion newOrientation = bodies[i]->getOrientation().rotateByVector(rotation);
//             bodies[i]->setOrientation(newOrientation);
//         }
//     }
// }
