#ifndef SHAPE_H
#define SHAPE_H

#include <vector>

namespace graphics
{
    class Shape
    {
    public:
        friend class Renderer;

    protected:
        /* 도형의 정점들 */
        std::vector<float> vertices;

        /* 표면을 구성하는 정점들의 인덱스 저장 */
        std::vector<unsigned int> polygonIndices;

        /* 테두리를 구성하는 정점들의 인덱스 저장 */
        std::vector<unsigned int> frameIndices;

        /* 표면을 렌더할 때 사용하는 VAO */
        unsigned int polygonVAO;

        /* 테두리를 렌더할 때 사용하는 VAO */
        unsigned int frameVAO;

    public:
        Shape() {}
        virtual ~Shape() {}

        /* 폴리곤, 테두리 VAO 설정 */
        void generateVAOs();

        /* 주어진 도형 데이터에 맞춰 정점 데이터를 생성한다.
            가변 인자를 활용하기 위해 인자를 double 로 선언한다 */
        virtual void generateVertices(double, ...) = 0;
    };

    class Box : public Shape
    {
    public:
        Box();
        void generateVertices(double, ...);
    };

    /* UV sphere */
    class Sphere : public Shape
    {
    public:
        static const int SECTOR_CNT = 36;
        static const int STACK_CNT = 18;

    public:
        Sphere();
        void generateVertices(double, ...);
        void generateIndices();
    };
} // namespace graphics


#endif // SHAPE_H
