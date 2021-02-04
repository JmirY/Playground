#include <physics/collision.h>

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
    if (distance - sphere.radius < 0)
        return true;
    else
        return false;
}

bool CollisionDetector::boxAndBox(const Box& box1, const Box& box2)
{

}

bool CollisionDetector::boxAndPlane(const Box& box, const Plane& plane)
{

}
