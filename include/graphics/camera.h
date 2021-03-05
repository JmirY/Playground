#ifndef CAMERA_H
#define CAMERA_H

#include "GLFW/glfw3.h"
#include "opengl/glm/glm.hpp"

namespace graphics
{
    /* 클래스 내 모든 벡터는 월드 좌표계 기준 */
    class Camera
    {
    protected:
        glm::vec3 position;
        glm::vec3 lookAtPoint;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;

        /* 카메라 옵션 */
        float fov;
        float panSensitivity;
        float rotateSensitivity;

    public:
        /* 생성자 */
        Camera();

        /* 현재 상태를 기준으로 뷰 매트릭스 리턴 */
        glm::mat4 getViewMatrix();

        /* 카메라 상하좌우 이동 */
        void pan(float xOffset, float yOffset, float zOffset);

        /* 카메라 뷰의 중앙을 기준으로 회전 */
        void rotate(glm::vec3 start, glm::vec3 end);

        /* FOV 값 수정을 통한 zoom 효과 */
        void zoom(float yOffset);

        /* getter */
        glm::vec3 getPosition() const;
        float getFov() const;

    private:
        /* 현재 상태를 기준으로 up & right 벡터 업데이트 */
        void updateCameraVectors();
    };    
} // namespace graphics

#endif // CAMERA_H
