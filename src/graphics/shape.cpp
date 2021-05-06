#include <graphics/opengl/glad/glad.h>
#include <graphics/shape.h>
#include <cmath>
#include <cstdarg>

using namespace graphics;

const float PI = 3.141592f;

void Shape::generateVAOs()
{
    /* 폴리곤 VAO 설정 */
    glGenVertexArrays(1, &polygonVAO);
    glBindVertexArray(polygonVAO);
    /* VBO 생성 후 바인드 */
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * vertices.size(),
        &vertices[0],
        GL_STATIC_DRAW
    );
    /* EBO 생성 후 바인드 */
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * polygonIndices.size(),
        &polygonIndices[0],
        GL_STATIC_DRAW
    );
    /* Attribute 설정 */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);

    /* 프레임 VAO 설정 */
    glGenVertexArrays(1, &frameVAO);
    glBindVertexArray(frameVAO);
    /* VBO 생성 후 바인드 */
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * vertices.size(),
        &vertices[0],
        GL_STATIC_DRAW
    );
    /* EBO 생성 후 바인드 */
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * frameIndices.size(),
        &frameIndices[0],
        GL_STATIC_DRAW
    );
    /* Attribute 설정 */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

Box::Box()
{
    generateVertices(0.5f, 0.5f, 0.5f);
    polygonIndices = {
        0, 1, 2,  0, 2, 3,  // 앞
        4, 5, 6,  4, 6, 7,  // 뒤
        3, 2, 6,  3, 6, 7,  // 위
        0, 1, 5,  0, 5, 4,  // 아래
        1, 5, 6,  1, 2, 6,  // 오른
        0, 4, 7,  0, 3, 7,  // 왼
    };
    frameIndices = {
        0, 1, 2, 3, 0,      // 앞
        4, 5, 6, 7, 4,      // 뒤
        5, 6, 2, 1, 5,      // 오른
        4, 0, 3, 7, 4,      // 왼
        7, 3, 2, 6, 7,      // 위
        4, 5, 1, 0, 4       // 아래
    };
    generateVAOs();
}

void Box::generateVertices(double value, ...)
{
    float halfX = (float)value;

    va_list args;
    va_start(args, value);

    float halfY = (float) va_arg(args, double);
    float halfZ = (float) va_arg(args, double);
    
    vertices.clear();
    vertices = {
        -halfX, -halfY, halfZ,  // bottom left
        halfX, -halfY, halfZ,   // bottom right
        halfX, halfY, halfZ,    // top right
        -halfX, halfY, halfZ,   // top left

        -halfX, -halfY, -halfZ, // bottom left
        halfX, -halfY, -halfZ,  // bottom right
        halfX, halfY, -halfZ,   // top right
        -halfX, halfY, -halfZ   // top left
    };
}

Sphere::Sphere()
{
    generateVertices(1.0f);
    generateIndices();
    generateVAOs();
}

/* 아래의 코드는 http://www.songho.ca/opengl/gl_sphere.html 을 참고했음을 밝힙니다 */

void Sphere::generateVertices(double radius, ...)
{
    float x, y, z, xy;
    float sectorStep = 2 * PI / SECTOR_CNT;
    float stackStep = PI / STACK_CNT;
    float sectorAngle, stackAngle;

    vertices.clear();
    for(int i = 0; i <= STACK_CNT; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= SECTOR_CNT; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
}

void Sphere::generateIndices()
{
    int k1, k2;
    for(int i = 0; i < STACK_CNT; ++i)
    {
        k1 = i * (SECTOR_CNT + 1);     // beginning of current stack
        k2 = k1 + SECTOR_CNT + 1;      // beginning of next stack

        for(int j = 0; j < SECTOR_CNT; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                polygonIndices.push_back(k1);
                polygonIndices.push_back(k2);
                polygonIndices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (STACK_CNT-1))
            {
                polygonIndices.push_back(k1 + 1);
                polygonIndices.push_back(k2);
                polygonIndices.push_back(k2 + 1);
            }

            // store indices for lines
            // vertical lines for all stacks, k1 => k2
        }
        frameIndices.push_back(k1);
        frameIndices.push_back(k2);
    }
}
