#ifndef QUATERNION_H
#define QUATERNION_H

#include "vector3.h"

namespace physics
{
    /* 크기가 1 인 단위 사원수만 3차원 회전을 표현할 수 있다 */
    class Quaternion
    {
    public:
        float w;
        float x;
        float y;
        float z;

    public:
        /* 생성자 */
        Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
        Quaternion(float _w, float _x, float _y, float _z)
            : w(_w), x(_x), y(_y), z(_z) {}
    
        /* 사원수의 크기를 1 로 만든다 */
        void normalize();

        /* 주어진 벡터만큼 회전한 사원수를 반환한다 */
        Quaternion rotateByVector(const Vector3& vec) const;

        /*****************
         * 연산자 오버로딩 *
         *****************/

        /* 사원수끼리 곱한다 */
        Quaternion operator*(const Quaternion& other) const;
        void operator*=(const Quaternion& other);
    };
} // namespace physics

#endif // QUATERNION_H
