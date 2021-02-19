#include <playground.h>

void Playground::run()
{
    /* 프레임 간 시간 측정 */
    double prevTime = glfwGetTime();
    double curTime, deltaTime;

    while (!glfwWindowShouldClose(renderer.window))
    {
        /* 시간 계산 */
        curTime = glfwGetTime();
        deltaTime = curTime - prevTime;
        prevTime = curTime;

        /* 물리 시뮬레이션 */
        simulator.simulate(deltaTime);

        /* Framebuffer 크기 퀴리 후 윈도우 크기를 갱신한다 */
        renderer.updateWindowSize();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* 배경 렌더 */
        renderer.renderBackground();

        /* 오브젝트 렌더 */
        Objects::iterator i = objects.begin();
        for (; i != objects.end(); ++i)
        {
            float modelMatrix[16];
            (*i)->body->getTransformMatrix(modelMatrix);
            renderer.renderObject((*i)->geometry, (*i)->color, modelMatrix);
        }

        glfwSwapBuffers(renderer.window);
        glfwPollEvents();
    }
}

void Playground::addObject(Geometry geometry)
{
    Object* newObject;

    if (geometry == SPHERE)
    {
        newObject = new SphereObject;
        newObject->geometry = SPHERE;
    }
    else if (geometry == BOX)
    {
        newObject = new BoxObject;
        newObject->geometry = BOX;
    }

    /* 색상은 무작위로 설정한다 */
    newObject->color = glm::vec3(
        arc4random() % 256 / 255.0f,
        arc4random() % 256 / 255.0f,
        arc4random() % 256 / 255.0f
    );

    /* 강체와 충돌체를 추가한다 */
    newObject->body = simulator.addPhysicsObject(geometry);

    objects.push_back(newObject);
}
