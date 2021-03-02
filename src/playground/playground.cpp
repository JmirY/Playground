#include <playground/playground.h>
#include <typeinfo>

Playground::Playground()
    : eventQueue(50), userInterface(renderer.getWindow(), renderer.getTextureBufferID())
{
    newObjectID = 0;
    // userInterface = gui::GUI(renderer.getWindow(), renderer.getTextureBufferID());
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

        /* GUI 이벤트 처리 */
        while (!eventQueue.isEmpty())
        {
            handleEvent(eventQueue.pop());
        }

        /* 물리 시뮬레이션 */
        simulator.simulate(deltaTime);

        renderer.updateWindowSize();
        
        renderer.bindSceneFrameBuffer();
        renderer.setSceneViewport();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* 배경 렌더 */
        renderer.renderBackground();
        /* 오브젝트 렌더 */
        for (auto& object : objects)
        {
            float modelMatrix[16];
            object.second->body->getTransformMatrix(modelMatrix);
            renderer.renderObject(object.second->id, object.second->color, modelMatrix, object.second->isSelected);
        }

        renderer.bindDefaultFrameBuffer();
        renderer.setWindowViewport();

        userInterface.renderAll(eventQueue, objects);

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

Playground::Objects::iterator Playground::removeObject(unsigned int id)
{
    /* 물리 데이터를 제거한다 */
    simulator.removePhysicsObject(id);

    /* 그래픽 데이터를 제거한다 */
    renderer.removeShape(id);

    /* 오브젝트를 objects 에서 제거하고 메모리에서 해제한다 */
    Objects::iterator objectIter = objects.find(id);
    delete objectIter->second;
    return objects.erase(objectIter);
}

void Playground::handleEvent(Event* event)
{
    if (typeid(*event) == typeid(ObjectSelectedEvent))
        handleObjectSelectedEvent(static_cast<ObjectSelectedEvent*>(event));

    else if (typeid(*event) == typeid(ObjectAddedEvent))
        handleObjectAddedEvent(static_cast<ObjectAddedEvent*>(event));

    else if (typeid(*event) == typeid(ObjectRemovedEvent))
        handleObjectRemovedEvent(static_cast<ObjectRemovedEvent*>(event));

    delete event;
}

void Playground::handleObjectAddedEvent(ObjectAddedEvent* event)
{
    addObject(event->geometry);
}

void Playground::handleObjectSelectedEvent(ObjectSelectedEvent* event)
{
    bool& isSelected = objects.find(event->id)->second->isSelected;
    isSelected = !isSelected;
}

void Playground::handleObjectRemovedEvent(ObjectRemovedEvent* event)
{
    Objects::iterator iter = objects.begin();
    for (; iter != objects.end();)
    {
        if (iter->second->isSelected)
            iter = removeObject(iter->second->id);
        else
            ++iter;
    }
}
