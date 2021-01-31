#include <physics/vector3.h>
#include <cmath>

using namespace physics;

void Vector3::normalize()
{
    float magnitudeInverse = 1.0f / magnitude();
    x *= magnitudeInverse;
    y *= magnitudeInverse;
    z *= magnitudeInverse;
}

float Vector3::magnitude() const
{
    float sum = x*x + y*y + z*z;
    return sqrtf(sum);
}

float Vector3::magnitudeSquared() const
{
    return x*x + y*y + z*z;
}

float Vector3::dot(const Vector3& other) const
{
    return x*other.x + y*other.y + z*other.z;
}

Vector3 Vector3::cross(const Vector3& other) const
{
    return Vector3(
        y*other.z - z*other.y,
        -x*other.z + z*other.x,
        x*other.y - y*other.x
    );
}

Vector3 Vector3::operator+(const Vector3& other) const
{
    return Vector3(x + other.x, y + other.y, z + other.z);
}

void Vector3::operator+=(const Vector3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
}

Vector3 Vector3::operator-(const Vector3& other) const
{
    return Vector3(x - other.x, y - other.y, z - other.z);
}

void Vector3::operator-=(const Vector3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

Vector3 Vector3::operator*(const float value) const
{
    return Vector3(x * value, y * value, z * value);
}

void Vector3::operator*=(const float value)
{
    x *= value;
    y *= value;
    z *= value;
}
