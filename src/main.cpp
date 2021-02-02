#include <graphics/renderer.h>
#include <physics/physics.h>

int main()
{
    /* 테스트 강체 */
    physics::RigidBody body;
    body.setMass(2.5f);
    body.setInertiaTensor(physics::Matrix3(0.25f));
    // body.setAcceleration(0.0f, -1.0f, 0.0f);
    body.setPosition(0.0f, 1.0f, 0.0f);
    // body.addForceAt(
    //     physics::Vector3(0.0f, 0.0f, -200.0f),
    //     physics::Vector3(0.5f, 0.0f, 0.0f)
    // );

    /* Renderer 생성 */
    graphics::Renderer renderer;

    /* 프레임 간 시간 측정 */
    double prevTime = glfwGetTime();
    double curTime, deltaTime;

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
        body.getTransformMatrix(modelMatrix);
        renderer.renderObject("Sphere", objectColor, modelMatrix);

        /* 시간 계산 */
        curTime = glfwGetTime();
        deltaTime = curTime - prevTime;
        prevTime = curTime;

        /* 물리 시뮬레이션 */
        body.integrate((float)deltaTime);

        glfwSwapBuffers(renderer.window);
        glfwPollEvents();
    }
    
    return 0;
}
