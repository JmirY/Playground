#ifndef RENDERER_H
#define RENDERER_H

#include "opengl/glad/glad.h"
#include "shader.h"
#include "camera.h"
#include "shape.h"
#include "../playground/geometry.h"
#include <GLFW/glfw3.h>
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
     * Renderer 클래스 속성 초기값 *
     ******************************/

    /* 초기 윈도우 너비 & 높이 */
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    /* Scene 너비 & 높이 */
    const int SCENE_WIDTH = 1024;
    const int SCENE_HEIGHT = 576;

    /* Perspective frustum 에서의 near & far 값 */
    const float PERSPECTIVE_NEAR = 0.1f;
    const float PERSPECTIVE_FAR = 100.0f;
    
    class Renderer
    {
    public:
        typedef std::map<unsigned int, Shape*> Shapes;

    private:
        int windowWidth, windowHeight;
    
        /* static 멤버 함수 convertScreenToWorld 에서 사용되므로 static 선언 */
        static int sceneWidth, sceneHeight;

        GLFWwindow *window;

        /* static 멤버 함수 convertScreenToWorld 에서 사용되므로 static 선언 */
        static Camera camera;
        Shader objectShader;

        /* Shape 포인터 저장 */
        Shapes shapes;

        /* 배경 VAO 의 ID */
        unsigned int backgroundVAO;

        /* 프레임 버퍼의 ID */
        unsigned int sceneFrameBufferID;

        /* 텍스처 버퍼의 ID */
        unsigned int textureBufferID;
        
    public:
        Renderer();
        ~Renderer();

        GLFWwindow* getWindow() const;
        unsigned int getTextureBufferID() const;

        Shape* addShape(unsigned int id, Geometry);
        void removeShape(unsigned int id);

        void renderObject(unsigned int id, glm::vec3 color, float modelMatrix[]);
        void renderBackground();

        /* 프레임 버퍼를 쿼리해 windowWidth & windowHeight 을 업데이트 */
        void updateWindowSize();

        void bindSceneFrameBuffer();
        void bindDefaultFrameBuffer();

        void setSceneViewport();
        void setWindowViewport();

        /* 마우스 입력에 따라 카메라를 조정한다 */
        static void cursorPosCallback(GLFWwindow *window, double xPos, double yPos);
        static void mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset);

    private:
        /* 스크린 좌표계 -> 월드 좌표계 변환 */
        static glm::vec3 convertScreenToWorld(glm::vec2 screenPt);
    };
} // namespace graphics

#endif // RENDERER_H