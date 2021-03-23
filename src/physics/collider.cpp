#include <physics/collider.h>
#include <cstdarg>

using namespace physics;

SphereCollider::SphereCollider(RigidBody* _body, float _radius)
{
    body = _body;
    radius = _radius;
}

void SphereCollider::setGeometricData(double value, ...)
{
    radius = value;
}

BoxCollider::BoxCollider(RigidBody* _body, float _halfX, float _halfY, float _halfZ)
{
    body = _body;
    halfSize.x = _halfX;
    halfSize.y = _halfY;
    halfSize.z = _halfZ;
}

void BoxCollider::setGeometricData(double value, ...)
{
    halfSize.x = value;

    va_list args;
    va_start(args, value);
    
    halfSize.y = va_arg(args, double);
    halfSize.z = va_arg(args, double);

    va_end(args);
}

PlaneCollider::PlaneCollider(Vector3 _normal, float _offset)
{
    normal = _normal;
    offset = _offset;
}
