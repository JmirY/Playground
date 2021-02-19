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

        /* 폴리곤을 구성하는 정점들의 인덱스 저장 */
        std::vector<unsigned int> polygonIndices;

        /* 프레임(테두리)을 구성하는 정점들의 인덱스 저장 */
        std::vector<unsigned int> frameIndices;

        /* 폴리곤을 렌더할 때 사용하는 VAO */
        unsigned int polygonVAO;

        /* 테두리를 렌더할 때 사용하는 VAO */
        unsigned int frameVAO;

    public:
        /* 생성자 */
        Shape() {}

        /* 폴리곤, 테두리 VAO 설정 */
        void setVAO();
    };

    /* 정육면체 */
    class Box : public Shape
    {
    public:
        Box();
        /* 주어진 데이터에 따라 정점을 생성한다 */
        void generateVertices(float halfX, float halfY, float halfZ);
    };

    /* 구 (UV sphere) */
    class Sphere : public Shape
    {
    public:
        Sphere();
        void generateVertices(float radius = 1.0f, int sectorCount = 36, int stackCount = 18);
        void generateIndices(int sectorCount = 36, int stackCount = 18);
    };
} // namespace graphics


#endif // SHAPE_H
