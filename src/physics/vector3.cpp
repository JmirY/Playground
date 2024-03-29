#include <physics/vector3.h>
#include <iostream>
#include <cmath>
#include <cfloat>

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

void Vector3::clear()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
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

const float Vector3::operator[](unsigned int idx) const
{
    switch (idx)
    {
    case 0:
        return x;

    case 1:
        return y;

    case 2:
        return z;
    
    default:
        std::cout << "Vector3::operator[]::Out of index" << std::endl;
        return FLT_MAX;
    }
}
