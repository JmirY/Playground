#include <graphics/camera.h>
#include <graphics/opengl/glm/gtc/quaternion.hpp>

using namespace graphics;

Camera::Camera()
{
    position = glm::vec3(10.0f, 10.0f, 10.0f);
    lookAtPoint = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    fov = 45.0f;
    panSensitivity = 0.01f;
    rotateSensitivity = 30.0f;
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, lookAtPoint, up);
}

glm::vec3 Camera::getViewPlaneNormal() const
{
    glm::vec3 viewPlaneNormal = glm::cross(up, right);
    return glm::normalize(viewPlaneNormal);
}

void Camera::pan(float xOffset, float yOffset, float zOffset)
{
    position -= right * (float) xOffset * panSensitivity;
    lookAtPoint -= right * (float) xOffset * panSensitivity;
    position -= up * (float) yOffset * panSensitivity;
    lookAtPoint -= up * (float) yOffset * panSensitivity;
    position -= glm::cross(up, right) * (float) zOffset * panSensitivity;
    lookAtPoint -= glm::cross(up, right) * (float) zOffset * panSensitivity;
}

void Camera::rotate(glm::vec3 axis, float angle)
{
    /* 카메라의 회전을 사원수로 표현 */
    glm::quat rotateQuat(
        glm::cos(angle / 2),
        axis.x * glm::sin(angle / 2),
        axis.y * glm::sin(angle / 2),
        axis.z * glm::sin(angle / 2)
    );
    /* 변환 행렬 계산 */
    glm::mat4 rotateMatrix = glm::mat4_cast(rotateQuat);
    glm::mat4 transformMatrix(1.0f);
    transformMatrix = glm::translate(transformMatrix, lookAtPoint);
    transformMatrix *= rotateMatrix;
    transformMatrix = glm::translate(transformMatrix, -lookAtPoint);
    
    glm::vec4 newCameraPos = transformMatrix * glm::vec4(position, 1.0f);
    position.x = newCameraPos.x / newCameraPos.w;
    position.y = newCameraPos.y / newCameraPos.w;
    position.z = newCameraPos.z / newCameraPos.w;
    
    updateCameraVectors();
}

void Camera::zoom(float yOffset)
{
    fov -= yOffset;
    if (fov < 1.0f)
        fov = 1.0f;
    else if (fov > 90.0f)
        fov = 90.0f;
}

glm::vec3 Camera::getPosition() const
{
    return position;
}

float Camera::getFov() const
{
    return fov;
}

void Camera::updateCameraVectors()
{
    right = glm::normalize(glm::cross(lookAtPoint - position, worldUp));
    up = glm::normalize(glm::cross(right, lookAtPoint - position));
}
