#include <graphics/renderer.h>
#include <physics/physics.h>

int main()
{
    /* 테스트 강체 */
    physics::RigidBody body;
    body.setMass(5.0f);
    body.setAcceleration(0.0f, -1.0f, 0.0f);

    /* Renderer 생성 */
    graphics::Renderer renderer;

    /* 메인 루프 */
    while (!glfwWindowShouldClose(renderer.window))
    {
        /* Framebuffer 크기 퀴리 후 윈도우 크기를 갱신한다 */
        renderer.updateWindowSize();

        /* local -> world 좌표계 변환 행렬 */
        static float modelMatrix[16];

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* 배경 렌더 */
        renderer.renderBackground();

        /* 오브젝트 렌더 */
        glm::vec3 objectColor(1.0f, 0.5f, 0.3f);

        glfwSwapBuffers(renderer.window);
        glfwPollEvents();
    }
    
    return 0;
}
