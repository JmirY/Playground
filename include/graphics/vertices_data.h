#ifndef VERTICES_DATA_H
#define VERTICES_DATA_H

#include <vector>

namespace graphics
{
    /************
     * 도형 데이터 *
     ************/

    /* 정육면체 */
    const std::vector<float> CUBE_VERTICES = {
        -0.5f, -0.5f, 0.5f,  // bottom left
        0.5f, -0.5f, 0.5f,   // bottom right
        0.5f, 0.5f, 0.5f,    // top right
        -0.5f, 0.5f, 0.5f,   // top left

        -0.5f, -0.5f, -0.5f, // bottom left
        0.5f, -0.5f, -0.5f,  // bottom right
        0.5f, 0.5f, -0.5f,   // top right
        -0.5f, 0.5f, -0.5f   // top left
    };
    const std::vector<unsigned int> CUBE_POLYGON_INDICES = {
        0, 1, 2,  0, 2, 3,  // 앞
        4, 5, 6,  4, 6, 7,  // 뒤
        3, 2, 6,  3, 6, 7,  // 위
        0, 1, 5,  0, 5, 4,  // 아래
        1, 5, 6,  1, 2, 6,  // 오른
        0, 4, 7,  0, 3, 7,  // 왼
    };
    const std::vector<unsigned int> CUBE_FRAME_INDICES = {
        0, 1, 2, 3, 0,      // 앞
        4, 5, 6, 7, 4,      // 뒤
        5, 6, 2, 1, 5,      // 오른
        4, 0, 3, 7, 4,      // 왼
        7, 3, 2, 6, 7,      // 위
        4, 5, 1, 0, 4       // 아래
    };
} // namespace graphics

#endif // VERTICES_DATA_H
