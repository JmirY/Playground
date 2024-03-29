#include <physics/quaternion.h>
#include <cmath>

using namespace physics;

void Quaternion::normalize()
{
    float magnitude = w*w + x*x + y*y + z*z;
    /* 크기가 0 이면 회전각 0 으로 설정한다 */
    if (magnitude == 0.0f)
    {
        w = 1.0f;
        return;
    }
    magnitude = 1.0f / sqrtf(magnitude);

    w *= magnitude;
    x *= magnitude;
    y *= magnitude;
    z *= magnitude;
}

Quaternion Quaternion::rotateByScaledVector(const Vector3& vec, const float scale) const
{
    return *this * Quaternion(0.0f, vec.x * scale, vec.y * scale, vec.z * scale);
}

Quaternion Quaternion::operator+(const Quaternion& other) const
{
    Quaternion result;

    result.w = w + other.w;
    result.x = x + other.x;
    result.y = y + other.y;
    result.z = z + other.z;

    return result;
}

void Quaternion::operator+=(const Quaternion& other)
{
    w += other.w;
    x += other.x;
    y += other.y;
    z += other.z;
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
    Quaternion result;
    
    result.w = w*other.w - x*other.x - y*other.y - z*other.z;
    result.x = w*other.x + x*other.w + y*other.z - z*other.y;
    result.y = w*other.y - x*other.z + y*other.w + z*other.x;
    result.z = w*other.z + x*other.y - y*other.x + z*other.w;

    return result;
}

void Quaternion::operator*=(const Quaternion& other)
{
    Quaternion result;
    
    result.w = w*other.w - x*other.x - y*other.y - z*other.z;
    result.x = w*other.x + x*other.w + y*other.z - z*other.y;
    result.y = w*other.y - x*other.z + y*other.w + z*other.x;
    result.z = w*other.z + x*other.y - y*other.x + z*other.w;

    *this = result;
}

Quaternion Quaternion::operator*(const float value) const
{
    Quaternion result;

    result.w = w * value;
    result.x = x * value;
    result.y = y * value;
    result.z = z * value;

    return result;
}

void Quaternion::operator*=(const float value)
{
    w *= value;
    x *= value;
    y *= value;
    z *= value;
}
