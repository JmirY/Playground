#include <physics/collision.h>
#include <vector>
#include <cmath>
#include <iostream>

using namespace physics;

Sphere::Sphere(RigidBody* _body, float _radius)
{
    body = _body;
    radius = _radius;
}

Box::Box(RigidBody* _body, float _halfX, float _halfY, float _halfZ)
{
    body = _body;
    halfX = _halfX;
    halfY = _halfY;
    halfZ = _halfZ;
}

Plane::Plane(Vector3 _normal, float _offset)
{
    normal = _normal;
    offset = _offset;
}

bool CollisionDetector::sphereAndBox(const Sphere& sphere, const Box& box)
{
    /* 구의 중심을 직육면체의 로컬 좌표계로 변환한다 */
    Matrix4 worldToLocal = box.body->getTransformMatrix().inverse();
    Vector3 sphereInBoxLocal = worldToLocal * sphere.body->getPosition();

    /* 구의 중심과 가장 가까운 직육면체 위의 점을 찾는다 */
    Vector3 closestPoint;
    /* x 축 성분 비교 */
    if (sphereInBoxLocal.x > box.halfX)
        closestPoint.x = box.halfX;
    else if (sphereInBoxLocal.x < -box.halfX)
        closestPoint.x = -box.halfX;
    else
        closestPoint.x = sphereInBoxLocal.x;
    /* y 축 성분 비교 */
    if (sphereInBoxLocal.y > box.halfY)
        closestPoint.y = box.halfY;
    else if (sphereInBoxLocal.y < -box.halfY)
        closestPoint.y = -box.halfY;
    else
        closestPoint.y = sphereInBoxLocal.y;
    /* z 축 성분 비교 */
    if (sphereInBoxLocal.z > box.halfZ)
        closestPoint.z = box.halfZ;
    else if (sphereInBoxLocal.z < -box.halfZ)
        closestPoint.z = -box.halfZ;
    else
        closestPoint.z = sphereInBoxLocal.z;

    /* 위의 결과와 구의 중심 사이의 거리가
        구의 반지름보다 작다면 충돌이 발생한 것이다 */
    float distanceSquared = (closestPoint - sphereInBoxLocal).magnitudeSquared();
    if (distanceSquared < sphere.radius*sphere.radius)
        return true;
    else
        return false;
}

bool CollisionDetector::sphereAndSphere(const Sphere& sphere1, const Sphere& sphere2)
{
    /* 두 구 사이의 거리를 구한다 */
    float distanceSquared =
        (sphere1.body->getPosition() - sphere2.body->getPosition()).magnitudeSquared();

    /* 두 구 사이의 거리가 두 구의 반지름의 합보다 작다면 충돌이 발생한 것이다 */
    float radiusSumSquared = (sphere1.radius + sphere2.radius)*(sphere1.radius + sphere2.radius);
    if (distanceSquared < radiusSumSquared)
        return true;
    else
        return false;
}

bool CollisionDetector::sphereAndPlane(const Sphere& sphere, const Plane& plane)
{
    /* 평면의 법선에 대한 구와 평면의 거리를 구한다 */
    float distance = plane.normal.dot(sphere.body->getPosition());
    distance -= plane.offset;

    /* 위 값이 구의 반지름보다 작다면 충돌이 발생한 것이다 */
    if (distance < sphere.radius)
        return true;
    else
        return false;
}

bool CollisionDetector::boxAndBox(const Box& box1, const Box& box2)
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

    /* 모든 축에 대해 겹침 검사 */
    for (int i = 0; i < axes.size(); ++i)
    {
        float penetration = overlap(box1, box2, axes[i]);
        if (penetration <= 0.0f)
            return false;
    }

    /* 모든 축에 걸쳐 겹침이 감지됐다면 충돌이 발생한 것이다 */
    return true;
}

bool CollisionDetector::boxAndPlane(const Box& box, const Plane& plane)
{
    /* 박스와 평면의 거리를 구한다 */
    float distance = plane.normal.dot(box.body->getPosition());
    distance -= plane.offset;
    
    /* 박스를 평면의 법선에 사영시킨다 */
    float projected = (box.body->getAxis(0) * box.halfX).dot(plane.normal)
        + (box.body->getAxis(1) * box.halfY).dot(plane.normal)
        + (box.body->getAxis(2) * box.halfZ).dot(plane.normal);

    /* 사영시킨 길이가 거리보다 크다면 충돌이 발생한 것이다 */
    if (projected > distance)
        return true;
    else
        return false;
}

float CollisionDetector::overlap(const Box& box1, const Box& box2, const Vector3& axis)
{
    /* 두 박스의 중심 간 거리를 계산한다 */
    Vector3 centerToCenter = box2.body->getPosition() - box1.body->getPosition();

    /* 두 박스를 주어진 축에 사영시킨 길이의 합을 계산한다 */
    float projectedSum = fabs((box1.body->getAxis(0) * box1.halfX).dot(axis))
        + fabs((box1.body->getAxis(1) * box1.halfY).dot(axis))
        + fabs((box1.body->getAxis(2) * box1.halfZ).dot(axis))
        + fabs((box2.body->getAxis(0) * box2.halfX).dot(axis))
        + fabs((box2.body->getAxis(1) * box2.halfY).dot(axis))
        + fabs((box2.body->getAxis(2) * box2.halfZ).dot(axis));

    /* "사영시킨 길이의 합 - 중심 간 거리" 가 겹친 정도이다 */
    return projectedSum - centerToCenter.magnitude();
}
