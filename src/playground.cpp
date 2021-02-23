#include <playground.h>

Playground::Playground()
{
    newObjectID = 0;
    userInterface = gui::GUI(renderer.getWindow());
}

void Playground::run()
{
    /* 프레임 간 시간 측정 */
    double prevTime = glfwGetTime();
    double curTime, deltaTime;

    while (!glfwWindowShouldClose(renderer.getWindow()))
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
        for (auto object : objects)
        {
            float modelMatrix[16];
            object.second->body->getTransformMatrix(modelMatrix);
            renderer.renderObject(object.second->id, object.second->color, modelMatrix);
        }

        glfwSwapBuffers(renderer.getWindow());
        glfwPollEvents();
    }
}

void Playground::addObject(Geometry geometry)
{
    Object* newObject;

    /* 주어진 도형에 따라 인스턴스를 생성한다 */
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

    /* id 를 부여한다 */
    newObject->id = newObjectID;
    
    /* 강체와 충돌체를 추가한다 */
    newObject->body = simulator.addRigidBody(newObjectID, geometry);
    newObject->collider = simulator.addCollider(newObjectID, geometry, newObject->body);

    /* 색상은 무작위로 설정한다 */
    newObject->color = glm::vec3(
        arc4random() % 256 / 255.0f,
        arc4random() % 256 / 255.0f,
        arc4random() % 256 / 255.0f
    );

    /* Shape 을 추가한다 */
    newObject->shape = renderer.addShape(newObjectID, geometry);

    objects[newObjectID] = newObject;
    ++newObjectID;
}

void Playground::removeObject(unsigned int id)
{
    /* 물리 데이터를 제거한다 */
    simulator.removePhysicsObject(id);

    /* 그래픽 데이터를 제거한다 */
    renderer.removeShape(id);

    /* 오브젝트를 objects 에서 제거하고 메모리에서 해제한다 */
    Objects::iterator objectIter = objects.find(id);
    delete objectIter->second;
    objects.erase(objectIter);
}
