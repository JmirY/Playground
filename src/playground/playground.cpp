#include <playground/playground.h>
#include <typeinfo>

Playground::Playground()
    : eventQueue(50), userInterface(renderer.getWindow(), renderer.getTextureBufferID())
{
    newObjectID = 0;
    isSimulating = true;
}

void Playground::run()
{
    /* 프레임 간 시간 측정 */
    double prevTime = glfwGetTime();
    double curTime, deltaTime;

    while (!glfwWindowShouldClose(renderer.getWindow()))
    {
        /* 키보드 입력 처리 */
        handleKeyboardInput();
        
        /* GUI 이벤트 처리 */
        while (!eventQueue.isEmpty())
        {
            handleEvent(eventQueue.pop());
        }

        /* 시간 계산 */
        curTime = glfwGetTime();
        deltaTime = curTime - prevTime;
        prevTime = curTime;

        /* 물리 시뮬레이션 */
        if (isSimulating)
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

        userInterface.renderAll(eventQueue, objects, isSimulating, selectedObjectIDs);

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

    else if (typeid(*event) == typeid(ObjectPositionChangedEvent))
        handleObjectPositionChangedEvent(static_cast<ObjectPositionChangedEvent*>(event));

    else if (typeid(*event) == typeid(ObjectVelocityChangedEvent))
        handleObjectVelocityChangedEvent(static_cast<ObjectVelocityChangedEvent*>(event));

    else if (typeid(*event) == typeid(SimulationStatusChangedEvent))
        handleSimulationStatusChangedEvent(static_cast<SimulationStatusChangedEvent*>(event));

    else if (typeid(*event) == typeid(ObjectGeometricDataChangedEvent))
        handleObjectGeometricDataChangedEvent(static_cast<ObjectGeometricDataChangedEvent*>(event));

    else if (typeid(*event) == typeid(ObjectRemovedEvent))
        handleObjectRemovedEvent(static_cast<ObjectRemovedEvent*>(event));

    else if (typeid(*event) == typeid(ObjectMassChangedEvent))
        handleObjectMassChangedEvent(static_cast<ObjectMassChangedEvent*>(event));

    else if (typeid(*event) == typeid(LeftMouseDraggedOnSceneEvent))
        handleLeftMouseDraggedOnSceneEvent(static_cast<LeftMouseDraggedOnSceneEvent*>(event));

    else if (typeid(*event) == typeid(RightMouseDraggedOnSceneEvent))
        handleRightMouseDraggedOnSceneEvent(static_cast<RightMouseDraggedOnSceneEvent*>(event));

    else if (typeid(*event) == typeid(MouseWheelOnSceneEvent))
        handleMouseWheelOnSceneEvent(static_cast<MouseWheelOnSceneEvent*>(event));

    else if (typeid(*event) == typeid(LeftMouseClickedOnSceneEvent))
        handleLeftMouseClickedOnSceneEvent(static_cast<LeftMouseClickedOnSceneEvent*>(event));

    else if (typeid(*event) == typeid(ObjectPositionFixedEvent))
        handleObjectPositionFixedEvent(static_cast<ObjectPositionFixedEvent*>(event));

    delete event;
}

void Playground::handleKeyboardInput()
{
    /* 프로그램 종료 */
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(renderer.getWindow(), GLFW_TRUE);
    
    /* 카메라 이동 */
    glm::vec3 offset(0.0f);
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        offset.x += -10.0f;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        offset.x += 10.0f;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        offset.z += -10.0f;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        offset.z += 10.0f;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_E) == GLFW_PRESS)
        offset.y += -10.0f;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
        offset.y += 10.0f;
    renderer.moveCamera(offset);
    
    /* 시뮬레이션 멈춤 혹은 재개 */
    static bool isRepeated = false;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (!isRepeated)
        {
            isSimulating = !isSimulating;
            isRepeated = true;
        }
    }
    else if (glfwGetKey(renderer.getWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE)
        isRepeated = false;
}

void Playground::clearSelectedObjectIDs()
{
    for (const auto& id : selectedObjectIDs)
        objects.find(id)->second->isSelected = false;
    
    selectedObjectIDs.clear();
}

void Playground::handleObjectAddedEvent(ObjectAddedEvent* event)
{
    addObject(event->geometry);
}

void Playground::handleObjectSelectedEvent(ObjectSelectedEvent* event)
{
    if (!event->isCtrlPressed)
    {
        for (const auto& id : selectedObjectIDs)
            objects.find(id)->second->isSelected = false;
        selectedObjectIDs.clear();
    }

    bool& isSelected = objects.find(event->id)->second->isSelected;
    if (isSelected) // TODO: 로직 개선
    {
        std::vector<unsigned int>::iterator it = selectedObjectIDs.begin();
        for (; it != selectedObjectIDs.end(); ++it)
        {
            if (*it == event->id)
            {
                selectedObjectIDs.erase(it);
                break;
            }
        }
        isSelected = false;
    }
    else
    {
        selectedObjectIDs.push_back(event->id);
        isSelected = true;
    }
}

void Playground::handleObjectRemovedEvent(ObjectRemovedEvent* event)
{
    for (const auto& id : selectedObjectIDs)
        removeObject(id);
    selectedObjectIDs.clear();
}

void Playground::handleSimulationStatusChangedEvent(SimulationStatusChangedEvent* event)
{
    if (isSimulating)
        isSimulating = false;
    else
        isSimulating = true;
}

void Playground::handleObjectPositionChangedEvent(ObjectPositionChangedEvent* event)
{
    float (&position)[3] = event->position;
    objects.find(event->id)->second->body->setPosition(position[0], position[1], position[2]);
}

void Playground::handleObjectVelocityChangedEvent(ObjectVelocityChangedEvent* event)
{
    float (&velocity)[3] = event->velocity;
    objects.find(event->id)->second->body->setVelocity(velocity[0], velocity[1], velocity[2]);
}

void Playground::handleObjectGeometricDataChangedEvent(ObjectGeometricDataChangedEvent* event)
{
    Object* object = objects.find(event->id)->second;
    float (&data)[3] = event->value;

    object->setGeometricData(data[0], data[1], data[2]);
    object->updateDerivedData();
}

void Playground::handleObjectMassChangedEvent(ObjectMassChangedEvent* event)
{
    Object* object = objects.find(event->id)->second;
    object->body->setMass(event->value);
    object->updateDerivedData();
}

void Playground::handleLeftMouseDraggedOnSceneEvent(LeftMouseDraggedOnSceneEvent* event)
{
    renderer.moveCamera(
        glm::vec3(event->xOffset, event->yOffset, 0.0f)
    );
}

void Playground::handleRightMouseDraggedOnSceneEvent(RightMouseDraggedOnSceneEvent* event)
{
    glm::vec3 dragStart = renderer.convertScreenToWorld(glm::vec2(event->prevX, event->prevY));
    glm::vec3 dragEnd = renderer.convertScreenToWorld(glm::vec2(event->curX, event->curY));

    /* 회전축 벡터 계산 */
    glm::vec3 axis = glm::cross(dragStart, dragEnd);
    axis = glm::normalize(axis);
    /* 두 벡터 사이각 계산 */
    dragEnd = glm::normalize(dragEnd);
    dragStart = glm::normalize(dragStart);
    float dotProduct = glm::dot(dragEnd, dragStart);
    if (dotProduct > 1.0f)
        dotProduct = 1.0f;
    float angle = -glm::acos(dotProduct);
    angle = glm::degrees(angle);
    
    renderer.rotateCamera(axis, angle);
}

void Playground::handleMouseWheelOnSceneEvent(MouseWheelOnSceneEvent* event)
{
    renderer.zoomCamera(event->value);
}

void Playground::handleLeftMouseClickedOnSceneEvent(LeftMouseClickedOnSceneEvent* event)
{
    /* 클릭 지점의 좌표를 월드 좌표로 변환한다 */
    glm::vec3 clickedPoint = renderer.convertScreenToWorld(
        glm::vec2(event->screenX, event->screenY)
    );
    physics::Vector3 origin(clickedPoint.x, clickedPoint.y, clickedPoint.z);
    
    /* Ray 의 방향을 계산한다 */
    glm::vec3 rayDirection = clickedPoint - renderer.getCameraPosition();
    rayDirection = glm::normalize(rayDirection);
    physics::Vector3 direction(rayDirection.x, rayDirection.y, rayDirection.z);
    
    /* Ray 와 부딪히는 오브젝트를 찾는다 */
    float minDistance = FLT_MAX;
    int minDistanceObjectID = -1;
    for (const auto& object : objects)
    {
        float distance = simulator.calcDistanceBetweenRayAndObject(
            origin,
            direction,
            object.second->id
        );

        if (distance > 0.0f && distance < minDistance)
        {
            minDistance = distance;
            minDistanceObjectID = object.second->id;
        }
    }

    if (minDistanceObjectID >= 0)
        eventQueue.push(new ObjectSelectedEvent(minDistanceObjectID, event->isCtrlPressed));
    else
        clearSelectedObjectIDs();
}

void Playground::handleObjectPositionFixedEvent(ObjectPositionFixedEvent* event)
{
    Object* target = objects.find(event->id)->second;
    if (event->hasToBeFixed)
    {
        target->isFixed = true;
        target->body->setInverseMass(0.0f);
        target->body->setVelocity(0.0f, 0.0f, 0.0f);
        target->body->setRotation(0.0f, 0.0f, 0.0f);
    }
    else
    {
        target->isFixed = false;
        target->body->setMass(5.0f);
    }
}
