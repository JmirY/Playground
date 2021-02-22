#ifndef RENDERER_H
#define RENDERER_H

#include "opengl/glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader.h"
#include "camera.h"
#include "shape.h"
#include "../geometry.h"
#include <map>

namespace graphics
{
    /**********************
     * 배경 vertices 데이터 *
     **********************/

    /* 바닥 그리드 */
    const std::vector<float> GRID_VERTICES = {
        /* z축과 평행한 선을 이루는 두 정점 */
        0.0f, 0.0f, 100.0f,
        0.0f, 0.0f, -100.0f
    };
    /* 그리드를 이루는 선 간격 */
    const float GRID_GAP = 1.0f;

    /******************************
     * Renderer 클래스 프로퍼티 초기값 *
     ******************************/

    /* 초기 윈도우 너비 & 높이 */
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    /* Perspective frustum 에서의 near & far 값 */
    const float PERSPECTIVE_NEAR = 0.1f;
    const float PERSPECTIVE_FAR = 100.0f;

    /******************
     * Renderer 클래스 *
     ******************/
    
    class Renderer
    {
    public:
        typedef std::map<unsigned int, Shape*> Shapes;

    private:
        /* static 멤버 함수 convertScreenToWorld 에서 사용되므로 static 선언 */
        static int windowWidth, windowHeight;

        GLFWwindow *window;

        /* static 멤버 함수 convertScreenToWorld 에서 사용되므로 static 선언 */
        static Camera* camera;
        Shader* objectShader;

        /* Shape 포인터 저장 */
        Shapes shapes;

        /* 배경 VAO */
        unsigned int backgroundVAO;
        
    public:
        Renderer();
        ~Renderer();

        GLFWwindow* getWindow();

        /* Shape 추가 */
        Shape* addShape(unsigned int id, Geometry);

        /* Shape 제거 */
        void removeShape(unsigned int id);

        /* 오브젝트 렌더 */
        void renderObject(unsigned int id, glm::vec3 color, float modelMatrix[]);

        /* 배경 렌더 */
        void renderBackground();
    
        /* 프레임 버퍼를 쿼리해 windowWidth & windowHeight 을 업데이트 */
        void updateWindowSize();

        /****************
         * glfw 콜백 함수 *
         ****************/
        // 마우스 입력에 따라 카메라 조정

        static void cursorPosCallback(GLFWwindow *window, double xPos, double yPos);
        static void mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset);

    private:
        /* 스크린 좌표계 -> 월드 좌표계 변환 */
        static glm::vec3 convertScreenToWorld(glm::vec2 screenPt);
    };
} // namespace graphics

#endif // RENDERER_H