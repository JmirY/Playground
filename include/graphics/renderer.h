#ifndef RENDERER_H
#define RENDERER_H

#include "opengl/glad/glad.h"
#include "shader.h"
#include "camera.h"
#include "shape.h"
#include "../playground/geometry.h"
#include "../playground/contact_info.h"
#include <GLFW/glfw3.h>
#include <map>

namespace graphics
{
    /* 지면 그리드 vertices */
    const std::vector<float> GRID_VERTICES = {
        /* z축과 평행한 선을 이루는 두 정점 */
        0.0f, 0.0f, 100.0f,
        0.0f, 0.0f, -100.0f
    };
    /* 그리드를 이루는 선 간격 */
    const float GRID_GAP = 1.0f;

    /* 충돌 법선 vertices */
    const std::vector<float> CONTACT_NORMAL_VERTICES = {
        0.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f
    };

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
    const float PERSPECTIVE_FAR = 300.0f;
    
    class Renderer
    {
    public:
        typedef std::map<unsigned int, Shape*> Shapes;

    private:
        int windowWidth, windowHeight;
        int sceneWidth, sceneHeight;

        GLFWwindow *window;

        Camera camera;
        Shader objectShader;

        /* Shape 포인터 저장 */
        Shapes shapes;

        /* 배경 VAO 의 ID */
        unsigned int backgroundVAO;

        /* 월드 y 축 VAO 의 ID */
        unsigned int worldYaxisVAO;

        /* 프레임 버퍼의 ID */
        unsigned int sceneFrameBufferID;

        /* 텍스처 버퍼의 ID */
        unsigned int textureBufferID;
        
    public:
        Renderer();
        ~Renderer();

        GLFWwindow* getWindow() const;
        unsigned int getTextureBufferID() const;
        glm::vec3 getCameraPosition() const;

        Shape* addShape(unsigned int id, Geometry);
        void removeShape(unsigned int id);

        void renderObject(
            unsigned int id,
            glm::vec3 color,
            float modelMatrix[],
            bool isSelected
        );
        void renderBackground();
        void renderContactInfo(ContactInfo*);
        void renderWorldAxisAt(int axisIdx, float posX, float posY, float posZ);
        void renderObjectAxis(int axisIdx, float modelMatrix[]);

        /* 프레임 버퍼를 쿼리해 windowWidth & windowHeight 을 업데이트 */
        void updateWindowSize();

        void bindSceneFrameBuffer();
        void bindDefaultFrameBuffer();

        void setSceneViewport();
        void setWindowViewport();

        void moveCamera(glm::vec3 offset);
        void rotateCamera(glm::vec3 axis, float angle);
        void zoomCamera(float degree);

        /* 스크린 좌표계 -> 월드 좌표계 변환 */
        glm::vec3 convertScreenToWorld(glm::vec2 screenPt);
    };
} // namespace graphics

#endif // RENDERER_H