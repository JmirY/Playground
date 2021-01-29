#ifndef MATRIX4_H
#define MATRIX4_H

namespace physics
{
    class Matrix4
    {
    public:
        float entries[16];

    public:
        /* 생성자 */
        Matrix4();            // 단위행렬로 초기화
        Matrix4(float value); // 행렬의 대각성분 (diagonal entry) 을 주어진 값으로 초기화
        Matrix4(float v1, float v2, float v3, float v4);

        /* 행렬의 대각성분을 주어진 값으로 설정한다
            나머지 성분은 0으로 설정한다 */
        void setDiagonalEntries(float value);

        /* 행렬의 전치행렬을 반환한다 */
        Matrix4 transpose() const;

        /* 행렬의 역행렬을 반환한다 */
        Matrix4 inverse() const;

        /*****************
         * 연산자 오버로딩 *
         *****************/

        /* 행렬끼리 더하기 */
        Matrix4 operator+(const Matrix4& other) const;
        void operator+=(const Matrix4& other);

        /* 행렬끼리 빼기 */
        Matrix4 operator-(const Matrix4& other) const;
        void operator-=(const Matrix4& other);

        /* 행렬끼리 곱하기 */
        Matrix4 operator*(const Matrix4& other) const;
        void operator*=(const Matrix4& other);
    };    
} // namespace physics

#endif // MATRIX4_H
