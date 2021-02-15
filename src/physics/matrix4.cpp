#include <physics/matrix4.h>
#include <iostream>

using namespace physics;

Matrix4::Matrix4()
{
    setDiagonal(1.0f);
}

Matrix4::Matrix4(float value)
{
    setDiagonal(value);
}

Matrix4::Matrix4(float v1, float v2, float v3, float v4)
{
    entries[0] = v1;
    entries[1] = 0.0f;
    entries[2] = 0.0f;
    entries[3] = 0.0f;

    entries[4] = 0.0f;
    entries[5] = v2;
    entries[6] = 0.0f;
    entries[7] = 0.0f;

    entries[8] = 0.0f;
    entries[9] = 0.0f;
    entries[10] = v3;
    entries[11] = 0.0f;

    entries[12] = 0.0f;
    entries[13] = 0.0f;
    entries[14] = 0.0f;
    entries[15] = v4;
}

void Matrix4::setDiagonal(float value)
{
    entries[0] = value;
    entries[1] = 0.0f;
    entries[2] = 0.0f;
    entries[3] = 0.0f;

    entries[4] = 0.0f;
    entries[5] = value;
    entries[6] = 0.0f;
    entries[7] = 0.0f;

    entries[8] = 0.0f;
    entries[9] = 0.0f;
    entries[10] = value;
    entries[11] = 0.0f;

    entries[12] = 0.0f;
    entries[13] = 0.0f;
    entries[14] = 0.0f;
    entries[15] = value;
}

Matrix4 Matrix4::transpose() const
{
    Matrix4 result(entries[0], entries[5], entries[10], entries[15]);

    result.entries[1] = entries[4];
    result.entries[2] = entries[8];
    result.entries[3] = entries[12];
    result.entries[4] = entries[1];
    result.entries[6] = entries[9];
    result.entries[7] = entries[13];

    result.entries[8] = entries[2];
    result.entries[9] = entries[6];
    result.entries[11] = entries[14];
    result.entries[12] = entries[3];
    result.entries[13] = entries[7];
    result.entries[14] = entries[11];

    return result;
}

Matrix4 Matrix4::inverse() const
{
    float inv[16];

    inv[0] = entries[5]  * entries[10] * entries[15] - 
             entries[5]  * entries[11] * entries[14] - 
             entries[9]  * entries[6]  * entries[15] + 
             entries[9]  * entries[7]  * entries[14] +
             entries[13] * entries[6]  * entries[11] - 
             entries[13] * entries[7]  * entries[10];

    inv[4] = -entries[4]  * entries[10] * entries[15] + 
              entries[4]  * entries[11] * entries[14] + 
              entries[8]  * entries[6]  * entries[15] - 
              entries[8]  * entries[7]  * entries[14] - 
              entries[12] * entries[6]  * entries[11] + 
              entries[12] * entries[7]  * entries[10];

    inv[8] = entries[4]  * entries[9] * entries[15] - 
             entries[4]  * entries[11] * entries[13] - 
             entries[8]  * entries[5] * entries[15] + 
             entries[8]  * entries[7] * entries[13] + 
             entries[12] * entries[5] * entries[11] - 
             entries[12] * entries[7] * entries[9];

    inv[12] = -entries[4]  * entries[9] * entries[14] + 
               entries[4]  * entries[10] * entries[13] +
               entries[8]  * entries[5] * entries[14] - 
               entries[8]  * entries[6] * entries[13] - 
               entries[12] * entries[5] * entries[10] + 
               entries[12] * entries[6] * entries[9];

    inv[1] = -entries[1]  * entries[10] * entries[15] + 
              entries[1]  * entries[11] * entries[14] + 
              entries[9]  * entries[2] * entries[15] - 
              entries[9]  * entries[3] * entries[14] - 
              entries[13] * entries[2] * entries[11] + 
              entries[13] * entries[3] * entries[10];

    inv[5] = entries[0]  * entries[10] * entries[15] - 
             entries[0]  * entries[11] * entries[14] - 
             entries[8]  * entries[2] * entries[15] + 
             entries[8]  * entries[3] * entries[14] + 
             entries[12] * entries[2] * entries[11] - 
             entries[12] * entries[3] * entries[10];

    inv[9] = -entries[0]  * entries[9] * entries[15] + 
              entries[0]  * entries[11] * entries[13] + 
              entries[8]  * entries[1] * entries[15] - 
              entries[8]  * entries[3] * entries[13] - 
              entries[12] * entries[1] * entries[11] + 
              entries[12] * entries[3] * entries[9];

    inv[13] = entries[0]  * entries[9] * entries[14] - 
              entries[0]  * entries[10] * entries[13] - 
              entries[8]  * entries[1] * entries[14] + 
              entries[8]  * entries[2] * entries[13] + 
              entries[12] * entries[1] * entries[10] - 
              entries[12] * entries[2] * entries[9];

    inv[2] = entries[1]  * entries[6] * entries[15] - 
             entries[1]  * entries[7] * entries[14] - 
             entries[5]  * entries[2] * entries[15] + 
             entries[5]  * entries[3] * entries[14] + 
             entries[13] * entries[2] * entries[7] - 
             entries[13] * entries[3] * entries[6];

    inv[6] = -entries[0]  * entries[6] * entries[15] + 
              entries[0]  * entries[7] * entries[14] + 
              entries[4]  * entries[2] * entries[15] - 
              entries[4]  * entries[3] * entries[14] - 
              entries[12] * entries[2] * entries[7] + 
              entries[12] * entries[3] * entries[6];

    inv[10] = entries[0]  * entries[5] * entries[15] - 
              entries[0]  * entries[7] * entries[13] - 
              entries[4]  * entries[1] * entries[15] + 
              entries[4]  * entries[3] * entries[13] + 
              entries[12] * entries[1] * entries[7] - 
              entries[12] * entries[3] * entries[5];

    inv[14] = -entries[0]  * entries[5] * entries[14] + 
               entries[0]  * entries[6] * entries[13] + 
               entries[4]  * entries[1] * entries[14] - 
               entries[4]  * entries[2] * entries[13] - 
               entries[12] * entries[1] * entries[6] + 
               entries[12] * entries[2] * entries[5];

    inv[3] = -entries[1] * entries[6] * entries[11] + 
              entries[1] * entries[7] * entries[10] + 
              entries[5] * entries[2] * entries[11] - 
              entries[5] * entries[3] * entries[10] - 
              entries[9] * entries[2] * entries[7] + 
              entries[9] * entries[3] * entries[6];

    inv[7] = entries[0] * entries[6] * entries[11] - 
             entries[0] * entries[7] * entries[10] - 
             entries[4] * entries[2] * entries[11] + 
             entries[4] * entries[3] * entries[10] + 
             entries[8] * entries[2] * entries[7] - 
             entries[8] * entries[3] * entries[6];

    inv[11] = -entries[0] * entries[5] * entries[11] + 
               entries[0] * entries[7] * entries[9] + 
               entries[4] * entries[1] * entries[11] - 
               entries[4] * entries[3] * entries[9] - 
               entries[8] * entries[1] * entries[7] + 
               entries[8] * entries[3] * entries[5];

    inv[15] = entries[0] * entries[5] * entries[10] - 
              entries[0] * entries[6] * entries[9] - 
              entries[4] * entries[1] * entries[10] + 
              entries[4] * entries[2] * entries[9] + 
              entries[8] * entries[1] * entries[6] - 
              entries[8] * entries[2] * entries[5];

    float determinant = entries[0] * inv[0] + entries[1] * inv[4] + entries[2] * inv[8] + entries[3] * inv[12];

    /* 행렬식이 0 이면 역행렬이 존재하지 않는다 */
    if (determinant == 0.0f)
    {
        std::cout << "MATRIX4::This matrix's inverse does not exist." << std::endl;
        return *this;
    }

    determinant = 1.0f / determinant;
    Matrix4 result;

    for (int i = 0; i < 16; i++)
        result.entries[i] = inv[i] * determinant;

    return result;
}

Matrix4 Matrix4::operator+(const Matrix4& other) const
{
    Matrix4 result;

    for (int i = 0; i < 16; ++i)
    {
        result.entries[i] = entries[i] + other.entries[i];
    }

    return result;
}

void Matrix4::operator+=(const Matrix4& other)
{
    for (int i = 0; i < 16; ++i)
    {
        entries[i] += other.entries[i];
    }
}

Matrix4 Matrix4::operator-(const Matrix4& other) const
{
    Matrix4 result;

    for (int i = 0; i < 16; ++i)
    {
        result.entries[i] = entries[i] - other.entries[i];
    }

    return result;
}

void Matrix4::operator-=(const Matrix4& other)
{
    for (int i = 0; i < 16; ++i)
    {
        entries[i] -= other.entries[i];
    }
}

Matrix4 Matrix4::operator*(const Matrix4& other) const
{
    Matrix4 result;

    result.entries[0] = entries[0] * other.entries[0] + entries[1] * other.entries[4] +
        entries[2] * other.entries[8] + entries[3] * other.entries[12];
    result.entries[1] = entries[0] * other.entries[1] + entries[1] * other.entries[5] +
        entries[2] * other.entries[9] + entries[3] * other.entries[13];
    result.entries[2] = entries[0] * other.entries[2] + entries[1] * other.entries[6] +
        entries[2] * other.entries[10] + entries[3] * other.entries[14];
    result.entries[3] = entries[0] * other.entries[3] + entries[1] * other.entries[7] +
        entries[2] * other.entries[11] + entries[3] * other.entries[15];

    result.entries[4] = entries[4] * other.entries[0] + entries[5] * other.entries[4] +
        entries[6] * other.entries[8] + entries[7] * other.entries[12];
    result.entries[5] = entries[4] * other.entries[1] + entries[5] * other.entries[5] +
        entries[6] * other.entries[9] + entries[7] * other.entries[13];
    result.entries[6] = entries[4] * other.entries[2] + entries[5] * other.entries[6] +
        entries[6] * other.entries[10] + entries[7] * other.entries[14];
    result.entries[7] = entries[4] * other.entries[3] + entries[5] * other.entries[7] +
        entries[6] * other.entries[11] + entries[7] * other.entries[15];

    result.entries[8] = entries[8] * other.entries[0] + entries[9] * other.entries[4] +
        entries[10] * other.entries[8] + entries[11] * other.entries[12];
    result.entries[9] = entries[8] * other.entries[1] + entries[9] * other.entries[5] +
        entries[10] * other.entries[9] + entries[11] * other.entries[13];
    result.entries[10] = entries[8] * other.entries[2] + entries[9] * other.entries[6] +
        entries[10] * other.entries[10] + entries[11] * other.entries[14];
    result.entries[11] = entries[8] * other.entries[3] + entries[9] * other.entries[7] +
        entries[10] * other.entries[11] + entries[11] * other.entries[15];

    result.entries[12] = entries[12] * other.entries[0] + entries[13] * other.entries[4] +
        entries[14] * other.entries[8] + entries[15] * other.entries[12];
    result.entries[13] = entries[12] * other.entries[1] + entries[13] * other.entries[5] +
        entries[14] * other.entries[9] + entries[15] * other.entries[13];
    result.entries[14] = entries[12] * other.entries[2] + entries[13] * other.entries[6] +
        entries[14] * other.entries[10] + entries[15] * other.entries[14];
    result.entries[15] = entries[12] * other.entries[3] + entries[13] * other.entries[7] +
        entries[14] * other.entries[11] + entries[15] * other.entries[15];

    return result;
}

void Matrix4::operator*=(const Matrix4& other)
{
    Matrix4 result;

    result.entries[0] = entries[0] * other.entries[0] + entries[1] * other.entries[4] +
        entries[2] * other.entries[8] + entries[3] * other.entries[12];
    result.entries[1] = entries[0] * other.entries[1] + entries[1] * other.entries[5] +
        entries[2] * other.entries[9] + entries[3] * other.entries[13];
    result.entries[2] = entries[0] * other.entries[2] + entries[1] * other.entries[6] +
        entries[2] * other.entries[10] + entries[3] * other.entries[14];
    result.entries[3] = entries[0] * other.entries[3] + entries[1] * other.entries[7] +
        entries[2] * other.entries[11] + entries[3] * other.entries[15];

    result.entries[4] = entries[4] * other.entries[0] + entries[5] * other.entries[4] +
        entries[6] * other.entries[8] + entries[7] * other.entries[12];
    result.entries[5] = entries[4] * other.entries[1] + entries[5] * other.entries[5] +
        entries[6] * other.entries[9] + entries[7] * other.entries[13];
    result.entries[6] = entries[4] * other.entries[2] + entries[5] * other.entries[6] +
        entries[6] * other.entries[10] + entries[7] * other.entries[14];
    result.entries[7] = entries[4] * other.entries[3] + entries[5] * other.entries[7] +
        entries[6] * other.entries[11] + entries[7] * other.entries[15];

    result.entries[8] = entries[8] * other.entries[0] + entries[9] * other.entries[4] +
        entries[10] * other.entries[8] + entries[11] * other.entries[12];
    result.entries[9] = entries[8] * other.entries[1] + entries[9] * other.entries[5] +
        entries[10] * other.entries[9] + entries[11] * other.entries[13];
    result.entries[10] = entries[8] * other.entries[2] + entries[9] * other.entries[6] +
        entries[10] * other.entries[10] + entries[11] * other.entries[14];
    result.entries[11] = entries[8] * other.entries[3] + entries[9] * other.entries[7] +
        entries[10] * other.entries[11] + entries[11] * other.entries[15];

    result.entries[12] = entries[12] * other.entries[0] + entries[13] * other.entries[4] +
        entries[14] * other.entries[8] + entries[15] * other.entries[12];
    result.entries[13] = entries[12] * other.entries[1] + entries[13] * other.entries[5] +
        entries[14] * other.entries[9] + entries[15] * other.entries[13];
    result.entries[14] = entries[12] * other.entries[2] + entries[13] * other.entries[6] +
        entries[14] * other.entries[10] + entries[15] * other.entries[14];
    result.entries[15] = entries[12] * other.entries[3] + entries[13] * other.entries[7] +
        entries[14] * other.entries[11] + entries[15] * other.entries[15];

    *this = result;
}

Vector3 Matrix4::operator*(const Vector3& vec) const
{
    float w = entries[12]*vec.x + entries[13]*vec.y + entries[14]*vec.z + entries[15];
    return Vector3(
        (entries[0]*vec.x + entries[1]*vec.y + entries[2]*vec.z + entries[3]) / w,
        (entries[4]*vec.x + entries[5]*vec.y + entries[6]*vec.z + entries[7]) / w,
        (entries[8]*vec.x + entries[9]*vec.y + entries[10]*vec.z + entries[11]) / w
    );
}

Matrix4 Matrix4::operator*(const float value) const
{
    Matrix4 result;

    for (int i = 0; i < 16; ++i)
    {
        result.entries[i] *= value;
    }

    return result;
}

void Matrix4::operator*=(const float value)
{
    for (int i = 0; i < 16; ++i)
    {
        entries[i] *= value;
    }
}
