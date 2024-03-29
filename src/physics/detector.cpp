#include <physics/detector.h>
#include <cmath>
#include <cfloat>

using namespace physics;

void CollisionDetector::detectCollision(
    std::vector<Contact*>& contacts,
    std::unordered_map<unsigned int, Collider*>& colliders,
    PlaneCollider& groundCollider
)
{
    for (auto i = colliders.begin(); i != colliders.end(); ++i)
    {
        Collider* colliderPtrI = i->second;
        for (auto j = std::next(i, 1); j != colliders.end(); ++j)
        {
            Collider* colliderPtrJ = j->second;
            if (typeid(*colliderPtrI) == typeid(SphereCollider))
            {
                SphereCollider* collider1 = static_cast<SphereCollider*>(colliderPtrI);
                if (typeid(*colliderPtrJ) == typeid(SphereCollider)) // 구 - 구 충돌
                {
                    SphereCollider* collider2 = static_cast<SphereCollider*>(colliderPtrJ);
                    sphereAndSphere(contacts, *collider1, *collider2);
                }
                else if (typeid(*colliderPtrJ) == typeid(BoxCollider)) // 구 - 직육면체 충돌
                {
                    BoxCollider* collider2 = static_cast<BoxCollider*>(colliderPtrJ);
                    sphereAndBox(contacts, *collider1, *collider2);
                }
            }
            else if (typeid(*colliderPtrI) == typeid(BoxCollider))
            {
                BoxCollider* collider1 = static_cast<BoxCollider*>(colliderPtrI);
                if (typeid(*colliderPtrJ) == typeid(SphereCollider)) // 구 - 직육면체 충돌
                {
                    SphereCollider* collider2 = static_cast<SphereCollider*>(colliderPtrJ);
                    sphereAndBox(contacts, *collider2, *collider1);
                }
                else if (typeid(*colliderPtrJ) == typeid(BoxCollider)) // 직육면체 - 직육면체 충돌
                {
                    BoxCollider* collider2 = static_cast<BoxCollider*>(colliderPtrJ);
                    boxAndBox(contacts, *collider1, *collider2);
                }
            }
        }

        /* 지면과의 충돌 검사 */
        if (typeid(*colliderPtrI) == typeid(SphereCollider))
        {
            SphereCollider* sphereCollider = static_cast<SphereCollider*>(i->second);
            sphereAndPlane(contacts, *sphereCollider, groundCollider);
        }
        else if (typeid(*colliderPtrI) == typeid(BoxCollider))
        {
            BoxCollider* boxCollider = static_cast<BoxCollider*>(i->second);
            boxAndPlane(contacts, *boxCollider, groundCollider);
        }
    }
}

bool CollisionDetector::sphereAndBox(
    std::vector<Contact*>& contacts,
    const SphereCollider& sphere,
    const BoxCollider& box
)
{
    /* 구의 중심을 직육면체의 로컬 좌표계로 변환한다 */
    Matrix4 worldToLocal = box.body->getTransformMatrix().inverse();
    Vector3 sphereInBoxLocal = worldToLocal * sphere.body->getPosition();

    /* 구의 중심과 가장 가까운 직육면체 위의 점을 찾는다 */
    Vector3 closestPoint;
    /* x 축 성분 비교 */
    if (sphereInBoxLocal.x > box.halfSize.x)
        closestPoint.x = box.halfSize.x;
    else if (sphereInBoxLocal.x < -box.halfSize.x)
        closestPoint.x = -box.halfSize.x;
    else
        closestPoint.x = sphereInBoxLocal.x;
    /* y 축 성분 비교 */
    if (sphereInBoxLocal.y > box.halfSize.y)
        closestPoint.y = box.halfSize.y;
    else if (sphereInBoxLocal.y < -box.halfSize.y)
        closestPoint.y = -box.halfSize.y;
    else
        closestPoint.y = sphereInBoxLocal.y;
    /* z 축 성분 비교 */
    if (sphereInBoxLocal.z > box.halfSize.z)
        closestPoint.z = box.halfSize.z;
    else if (sphereInBoxLocal.z < -box.halfSize.z)
        closestPoint.z = -box.halfSize.z;
    else
        closestPoint.z = sphereInBoxLocal.z;

    /* 위의 결과와 구의 중심 사이의 거리가
        구의 반지름보다 작다면 충돌이 발생한 것이다 */
    float distanceSquared = (closestPoint - sphereInBoxLocal).magnitudeSquared();
    if (distanceSquared < sphere.radius*sphere.radius)
    {
        /* 구에 가장 가까운 직육면체 위의 점을 월드 좌표계로 변환한다 */
        Vector3 closestPointWorld = box.body->getTransformMatrix() * closestPoint;

        /* 충돌 정보를 생성한다 */
        Contact* newContact = new Contact;
        newContact->bodies[0] = sphere.body;
        newContact->bodies[1] = box.body;
        newContact->normal = sphere.body->getPosition() - closestPointWorld;
        newContact->normal.normalize();
        newContact->contactPoint[0] =
            new Vector3(sphere.body->getPosition() - newContact->normal * sphere.radius);
        newContact->contactPoint[1] = new Vector3(closestPointWorld);
        newContact->penetration = sphere.radius - sqrtf(distanceSquared);
        newContact->restitution = objectRestitution;
        newContact->friction = friction;
        newContact->normalImpulseSum = 0.0f;
        newContact->tangentImpulseSum1 = 0.0f;
        newContact->tangentImpulseSum2 = 0.0f;

        contacts.push_back(newContact);
        return true;
    }
    else
        return false;
}

bool CollisionDetector::sphereAndSphere(
    std::vector<Contact*>& contacts,
    const SphereCollider& sphere1,
    const SphereCollider& sphere2
)
{
    /* 두 구 사이의 거리를 구한다 */
    float distanceSquared =
        (sphere1.body->getPosition() - sphere2.body->getPosition()).magnitudeSquared();

    /* 두 구 사이의 거리가 두 구의 반지름의 합보다 작다면 충돌이 발생한 것이다 */
    float radiusSum = sphere1.radius + sphere2.radius;
    if (distanceSquared < radiusSum*radiusSum)
    {
        Vector3 centerToCenter = sphere1.body->getPosition() - sphere2.body->getPosition();
        centerToCenter.normalize();
        
        /* 충돌 정보를 생성한다 */
        Contact* newContact = new Contact;
        newContact->bodies[0] = sphere1.body;
        newContact->bodies[1] = sphere2.body;
        newContact->normal = centerToCenter;
        newContact->contactPoint[0] =
            new Vector3(sphere1.body->getPosition() - centerToCenter * sphere1.radius);
        newContact->contactPoint[1] =
            new Vector3(sphere2.body->getPosition() + centerToCenter * sphere2.radius);
        newContact->penetration = radiusSum - sqrtf(distanceSquared);
        newContact->restitution = objectRestitution;
        newContact->friction = friction;
        newContact->normalImpulseSum = 0.0f;
        newContact->tangentImpulseSum1 = 0.0f;
        newContact->tangentImpulseSum2 = 0.0f;

        contacts.push_back(newContact);
        return true;
    }
    else
        return false;
}

bool CollisionDetector::sphereAndPlane(
    std::vector<Contact*>& contacts,
    const SphereCollider& sphere,
    const PlaneCollider& plane
)
{
    /* 평면의 법선에 대한 구와 평면의 거리를 구한다 */
    float distance = plane.normal.dot(sphere.body->getPosition());
    distance -= plane.offset;

    /* 위 값이 구의 반지름보다 작다면 충돌이 발생한 것이다 */
    if (distance < sphere.radius)
    {
        /* 충돌 정보를 생성한다 */
        Contact* newContact = new Contact;
        newContact->bodies[0] = sphere.body;
        newContact->bodies[1] = nullptr;
        newContact->normal = plane.normal;
        newContact->contactPoint[0] = new Vector3(sphere.body->getPosition() - plane.normal * distance);
        newContact->contactPoint[1] = nullptr;
        newContact->penetration = sphere.radius - distance;
        newContact->restitution = groundRestitution;
        newContact->friction = friction;
        newContact->normalImpulseSum = 0.0f;
        newContact->tangentImpulseSum1 = 0.0f;
        newContact->tangentImpulseSum2 = 0.0f;

        contacts.push_back(newContact);
        return true;
    }
    else
        return false;
}

bool CollisionDetector::boxAndBox(
    std::vector<Contact*>& contacts,
    const BoxCollider& box1,
    const BoxCollider& box2
)
{
    /* 겹침 검사의 기준이 될 축 저장 */
    std::vector<Vector3> axes;

    /* box1 의 세 축 저장 */
    for (int i = 0; i < 3; ++i)
        axes.push_back(box1.body->getAxis(i));

    /* box2 의 세 축 저장 */
    for (int i = 0; i < 3; ++i)
        axes.push_back(box2.body->getAxis(i));

    /* 각 축 사이의 외적 저장 */
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            Vector3 crossProduct = axes[i].cross(axes[3 + j]);
            crossProduct.normalize();
            axes.push_back(crossProduct);
        }
    }

    float minPenetration = FLT_MAX;
    int minAxisIdx = 0;

    /* 모든 축에 대해 겹침 검사 */
    for (int i = 0; i < axes.size(); ++i)
    {
        float penetration = calcPenetration(box1, box2, axes[i]);

        /* 한 축이라도 겹치지 않으면 충돌이 발생하지 않은 것이다 */
        if (penetration <= 0.0f)
            return false;

        /* 가장 적게 겹치는 정도와 그때의 기준 축을 추적한다 */
        if (penetration <= minPenetration)
        {
            minPenetration = penetration;
            minAxisIdx = i;
        }
    }

    /* 모든 축에 걸쳐 겹침이 감지됐다면 충돌이 발생한 것이다 */
    Contact* newContact = new Contact;
    newContact->bodies[0] = box1.body;
    newContact->bodies[1] = box2.body;
    newContact->penetration = minPenetration;
    newContact->restitution = objectRestitution;
    newContact->friction = friction;
    newContact->normalImpulseSum = 0.0f;
    newContact->tangentImpulseSum1 = 0.0f;
    newContact->tangentImpulseSum2 = 0.0f;

    /* 충돌 법선을 방향에 유의하여 설정한다 */
    Vector3 centerToCenter = box2.body->getPosition() - box1.body->getPosition();
    if (axes[minAxisIdx].dot(centerToCenter) > 0)
        newContact->normal = axes[minAxisIdx] * -1.0f;
    else
        newContact->normal = axes[minAxisIdx];

    /* 충돌 지점을 찾는다 */
    if (minAxisIdx < 6) // 면-점 접촉일 때
    {
        calcContactPointOnPlane(box1, box2, minAxisIdx, newContact);
    }
    else // 선-선 접촉일 때
    {
        calcContactPointOnLine(box1, box2, minAxisIdx, newContact);
    }
    
    contacts.push_back(newContact);
    return true;
}

bool CollisionDetector::boxAndPlane(
    std::vector<Contact*>& contacts,
    const BoxCollider& box,
    const PlaneCollider& plane
)
{
    /* 직육면체를 이루는 로컬 좌표계의 정점들 */
    Vector3 vertices[8];
    vertices[0] = Vector3(-box.halfSize.x, box.halfSize.y, box.halfSize.z);
    vertices[1] = Vector3(-box.halfSize.x, -box.halfSize.y, box.halfSize.z);
    vertices[2] = Vector3(box.halfSize.x, -box.halfSize.y, box.halfSize.z);
    vertices[3] = Vector3(box.halfSize.x, box.halfSize.y, box.halfSize.z);

    vertices[4] = Vector3(-box.halfSize.x, box.halfSize.y, -box.halfSize.z);
    vertices[5] = Vector3(-box.halfSize.x, -box.halfSize.y, -box.halfSize.z);
    vertices[6] = Vector3(box.halfSize.x, -box.halfSize.y, -box.halfSize.z);
    vertices[7] = Vector3(box.halfSize.x, box.halfSize.y, -box.halfSize.z);

    /* 정점들을 월드 좌표계로 변환한다 */
    for (int i = 0; i < 8; ++i)
        vertices[i] = box.body->getTransformMatrix() * vertices[i];

    /* 반환값으로 사용한다 */
    bool hasContacted = false;

    /* 모든 정점들의 충돌 여부를 검사한다 */
    for (int i = 0; i < 8; ++i)
    {
        float distance = plane.normal.dot(vertices[i]);
        distance -= plane.offset;

        if (distance < 0)
        {
            /* 충돌을 생성한다 */
            Contact* newContact = new Contact;
            newContact->bodies[0] = box.body;
            newContact->bodies[1] = nullptr;
            newContact->normal = plane.normal;
            newContact->contactPoint[0] = new Vector3(vertices[i]);
            newContact->contactPoint[1] = nullptr;
            newContact->penetration = -distance;
            newContact->restitution = groundRestitution;
            newContact->friction = friction;
            newContact->normalImpulseSum = 0.0f;
            newContact->tangentImpulseSum1 = 0.0f;
            newContact->tangentImpulseSum2 = 0.0f;

            contacts.push_back(newContact);
            hasContacted = true;
        }
    }

    return hasContacted;
}

float CollisionDetector::rayAndSphere(
    const Vector3& origin,
    const Vector3& direction,
    const SphereCollider& sphere
)
{
    Vector3 originToSphere = sphere.body->getPosition() - origin;
    float originToSphereProjected = originToSphere.dot(direction);
    float orthogonalDistanceSquared =
        originToSphere.magnitudeSquared() - originToSphereProjected * originToSphereProjected;
    if (orthogonalDistanceSquared > sphere.radius*sphere.radius)
        return -1.0f;

    float t1 = sqrtf(sphere.radius*sphere.radius - orthogonalDistanceSquared);
    float hitPointDistance = originToSphereProjected - t1;

    return hitPointDistance;
}

float CollisionDetector::rayAndBox(
    const Vector3& origin,
    const Vector3& direction,
    const BoxCollider& box
)
{
    Vector3 originToBox = box.body->getPosition() - origin;
    float tNearMax = 0.0f;     // 가까운 평면과 ray 의 원점 사이의 거리 최댓값
    float tFarMin = FLT_MAX;  // 먼 평면과 ray 의 원점 사이의 거리 최솟값
    
    for (int i = 0; i < 3; ++i)
    {
        Vector3 axis = box.body->getAxis(i);
        float originToBoxProjected = axis.dot(originToBox);
        float rayDirectionProjected = axis.dot(direction);

        if (fabs(rayDirectionProjected) > 0.001f)
        {
            float t1 = (originToBoxProjected - box.halfSize[i]) / rayDirectionProjected;
            float t2 = (originToBoxProjected + box.halfSize[i]) / rayDirectionProjected;
            /* t1 이 가까운 평면에 대한 값이어야 하므로
                t1 가 t2 보다 크다면 두 변수를 swap 한다 */
            if (t1 > t2)
            {
                float temp = t1;
                t1 = t2;
                t2 = temp;
            }
            if (t1 > tNearMax)
                tNearMax = t1;
            if (t2 < tFarMin)
                tFarMin = t2;
            if (tFarMin < tNearMax)
                return -1.0f;
        }
        else
        {
            if (-originToBoxProjected - box.halfSize[i] > 0.0f
                    || -originToBoxProjected + box.halfSize[i] < 0.0f)
                return -1.0f;
        }
    }

    return tNearMax;
}

float CollisionDetector::calcPenetration(const BoxCollider& box1, const BoxCollider& box2, const Vector3& axis)
{
    /* 두 박스의 중심 간 거리를 계산한다 */
    Vector3 centerToCenter = box2.body->getPosition() - box1.body->getPosition();
    float projectedCenterToCenter = abs(centerToCenter.dot(axis));

    /* 두 박스를 주어진 축에 사영시킨 길이의 합을 계산한다 */
    float projectedSum = abs((box1.body->getAxis(0) * box1.halfSize.x).dot(axis))
        + abs((box1.body->getAxis(1) * box1.halfSize.y).dot(axis))
        + abs((box1.body->getAxis(2) * box1.halfSize.z).dot(axis))
        + abs((box2.body->getAxis(0) * box2.halfSize.x).dot(axis))
        + abs((box2.body->getAxis(1) * box2.halfSize.y).dot(axis))
        + abs((box2.body->getAxis(2) * box2.halfSize.z).dot(axis));

    /* "사영시킨 길이의 합 - 중심 간 거리" 가 겹친 정도이다 */
    return projectedSum - projectedCenterToCenter;
}

void CollisionDetector::calcContactPointOnPlane(
    const BoxCollider& box1,
    const BoxCollider& box2,
    int minAxisIdx,
    Contact* contact
)
{
    /* 충돌 정점 */
    Vector3* contactPoint1;
    Vector3* contactPoint2;

    if (minAxisIdx < 3) // 충돌면이 box1 의 면일 때
    {
        contactPoint2 = new Vector3(box2.halfSize.x, box2.halfSize.y, box2.halfSize.z);

        if (box2.body->getAxis(0).dot(contact->normal) < 0)
            contactPoint2->x *= -1.0f;
        if (box2.body->getAxis(1).dot(contact->normal) < 0)
            contactPoint2->y *= -1.0f;
        if (box2.body->getAxis(2).dot(contact->normal) < 0)
            contactPoint2->z *= -1.0f;

        /* 월드 좌표로 변환한다 */
        *contactPoint2 = box2.body->getTransformMatrix() * (*contactPoint2);

        contactPoint1 = new Vector3(*contactPoint2 - contact->normal * contact->penetration);
    }
    else // 충돌면이 box2 의 면일 때
    {
        contactPoint1 = new Vector3(box1.halfSize.x, box1.halfSize.y, box1.halfSize.z);

        if (box1.body->getAxis(0).dot(contact->normal) > 0)
            contactPoint1->x *= -1.0f;
        if (box1.body->getAxis(1).dot(contact->normal) > 0)
            contactPoint1->y *= -1.0f;
        if (box1.body->getAxis(2).dot(contact->normal) > 0)
            contactPoint1->z *= -1.0f;

        /* 월드 좌표로 변환한다 */
        *contactPoint1 = box1.body->getTransformMatrix() * (*contactPoint1);

        contactPoint2 = new Vector3(*contactPoint1 - contact->normal * contact->penetration);
    }

    contact->contactPoint[0] = contactPoint1;
    contact->contactPoint[1] = contactPoint2;
}

void CollisionDetector::calcContactPointOnLine(
    const BoxCollider& box1,
    const BoxCollider& box2,
    int minAxisIdx,
    Contact* contact
)
{
    /* 접촉한 변 위의 정점을 찾는다 */
    Vector3 vertexOne(box1.halfSize.x, box1.halfSize.y, box1.halfSize.z);
    Vector3 vertexTwo(box2.halfSize.x, box2.halfSize.y, box2.halfSize.z);

    if (box1.body->getAxis(0).dot(contact->normal) > 0)
        vertexOne.x *= -1.0f;
    if (box1.body->getAxis(1).dot(contact->normal) > 0)
        vertexOne.y *= -1.0f;
    if (box1.body->getAxis(2).dot(contact->normal) > 0)
        vertexOne.z *= -1.0f;

    if (box2.body->getAxis(0).dot(contact->normal) < 0)
        vertexTwo.x *= -1.0f;
    if (box2.body->getAxis(1).dot(contact->normal) < 0)
        vertexTwo.y *= -1.0f;
    if (box2.body->getAxis(2).dot(contact->normal) < 0)
        vertexTwo.z *= -1.0f;

    /* 변의 방향을 찾는다 */
    Vector3 directionOne, directionTwo;
    
    switch (minAxisIdx)
    {
    case 6: // box1 의 x 축 X box2 의 x 축
        directionOne = box1.body->getAxis(0);
        if (vertexOne.x > 0) directionOne *= -1.0f;
        directionTwo = box2.body->getAxis(0);
        if (vertexTwo.x > 0) directionTwo *= -1.0f;
        break;

    case 7: // box1 의 x 축 X box2 의 y 축
        directionOne = box1.body->getAxis(0);
        if (vertexOne.x > 0) directionOne *= -1.0f;
        directionTwo = box2.body->getAxis(1);
        if (vertexTwo.y > 0) directionTwo *= -1.0f;
        break;

    case 8: // box1 의 x 축 X box2 의 z 축
        directionOne = box1.body->getAxis(0);
        if (vertexOne.x > 0) directionOne *= -1.0f;
        directionTwo = box2.body->getAxis(2);
        if (vertexTwo.z > 0) directionTwo *= -1.0f;
        break;

    case 9: // box1 의 y 축 X box2 의 x 축
        directionOne = box1.body->getAxis(1);
        if (vertexOne.y > 0) directionOne *= -1.0f;
        directionTwo = box2.body->getAxis(0);
        if (vertexTwo.x > 0) directionTwo *= -1.0f;
        break;

    case 10: // box1 의 y 축 X box2 의 y 축
        directionOne = box1.body->getAxis(1);
        if (vertexOne.y > 0) directionOne *= -1.0f;
        directionTwo = box2.body->getAxis(1);
        if (vertexTwo.y > 0) directionTwo *= -1.0f;
        break;
    
    case 11: // box1 의 y 축 X box2 의 z 축
        directionOne = box1.body->getAxis(1);
        if (vertexOne.y > 0) directionOne *= -1.0f;
        directionTwo = box2.body->getAxis(2);
        if (vertexTwo.z > 0) directionTwo *= -1.0f;
        break;
    
    case 12: // box1 의 z 축 X box2 의 x 축
        directionOne = box1.body->getAxis(2);
        if (vertexOne.z > 0) directionOne *= -1.0f;
        directionTwo = box2.body->getAxis(0);
        if (vertexTwo.x > 0) directionTwo *= -1.0f;
        break;
    
    case 13: // box1 의 z 축 X box2 의 y 축
        directionOne = box1.body->getAxis(2);
        if (vertexOne.z > 0) directionOne *= -1.0f;
        directionTwo = box2.body->getAxis(1);
        if (vertexTwo.y > 0) directionTwo *= -1.0f;
        break;
    
    case 14: // box1 의 z 축 X box2 의 z 축
        directionOne = box1.body->getAxis(2);
        if (vertexOne.z > 0) directionOne *= -1.0f;
        directionTwo = box2.body->getAxis(2);
        if (vertexTwo.z > 0) directionTwo *= -1.0f;
        break;
    
    default:
        break;
    }

    /* 정점을 월드 좌표계로 변환한다 */
    vertexOne = box1.body->getTransformMatrix() * vertexOne;
    vertexTwo = box2.body->getTransformMatrix() * vertexTwo;

    /* box2 의 변과 가장 가까운 box1 위의 점을 찾는다 */
    float k = directionOne.dot(directionTwo);
    Vector3* closestPointOne =
        new Vector3(vertexOne + directionOne * ((vertexTwo-vertexOne).dot(directionOne-directionTwo*k)/(1-k*k)));
    
    /* box1 의 변과 가장 가까운 box2 위의 점을 찾는다 */
    Vector3* closestPointTwo =
        new Vector3(vertexTwo + directionTwo * ((*closestPointOne-vertexTwo).dot(directionTwo)));

    contact->contactPoint[0] = closestPointOne;
    contact->contactPoint[1] = closestPointTwo;
}
