#ifndef VECTOR3_H
#define VECTOR3_H

namespace physics
{
    struct Vector3
    {
        float x;
        float y;
        float z;

        /* 생성자 */
        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        /* 벡터를 정규화한다 */
        void normalize();

        /* 벡터의 크기를 반환한다 */
        float magnitude() const;
        /* 벡터 크기의 제곱을 반환한다 */
        float magnitudeSquared() const;

        /* 주어진 벡터와의 내적을 반환한다 */
        float dot(const Vector3& other) const;

        /* 주어진 벡터와의 외적을 반환한다 */
        Vector3 cross(const Vector3& other) const;

        /* 벡터를 0 으로 초기화한다 */
        void clear();

        /*****************
         * 연산자 오버로딩 *
         *****************/

        /* 벡터끼리 더하기 */
        Vector3 operator+(const Vector3& other) const;
        void operator+=(const Vector3& other);

        /* 벡터끼리 빼기 */
        Vector3 operator-(const Vector3& other) const;
        void operator-=(const Vector3& other);

        /* 벡터와 스칼라끼리 곱하기 */
        Vector3 operator*(const float value) const;
        void operator*=(const float value);

        /* 인덱스로 접근하기 */
        const float operator[](unsigned int idx) const;
    };
} // namespace physics

#endif // VECTOR3_H
