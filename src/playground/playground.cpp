#include <playground/playground.h>
#include <typeinfo>
#include <cmath>

const float PI = 3.141592f;

Playground::Playground()
    : eventQueue(50), userInterface(renderer.getWindow(), renderer.getTextureBufferID())
{
    newObjectID = 1;
    isSimulating = true;
    shouldRenderContactInfo = false;
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
        
        /* 시간 계산 */
        curTime = glfwGetTime();
        deltaTime = curTime - prevTime;
        prevTime = curTime;

        /* 물리 시뮬레이션 */
        std::vector<ContactInfo*> contactInfo;
        if (isSimulating)
            simulator.simulate(deltaTime, contactInfo);

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
            renderer.renderObject(
                object.second->id,
                object.second->color,
                modelMatrix,
                object.second->isSelected,
                object.second->isFixed
            );
        }

        /* 선택된 오브젝트의 로컬축 렌더 */
        if (selectedObjectIDs.size() == 1)
        {
            float modelMatrix[16];
            objects.find(selectedObjectIDs[0])->second->body->getTransformMatrix(modelMatrix);
            renderer.renderObjectAxis(0, modelMatrix);
            renderer.renderObjectAxis(1, modelMatrix);
            renderer.renderObjectAxis(2, modelMatrix);
        }

        /* 충돌점 렌더 */
        for (auto& info : contactInfo)
        {
            if (shouldRenderContactInfo)
                renderer.renderContactInfo(info);
            delete info;
        }
        contactInfo.clear();

        /* GUI 이벤트 처리 */
        while (!eventQueue.isEmpty())
        {
            handleEvent(eventQueue.pop());
        }

        renderer.bindDefaultFrameBuffer();
        renderer.setWindowViewport();

        userInterface.renderAll(eventQueue, objects, isSimulating, selectedObjectIDs);

        glfwSwapBuffers(renderer.getWindow());
        glfwPollEvents();
    }
}

unsigned int Playground::addObject(Geometry geometry, float posX, float posY, float posZ)
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
    newObject->body = simulator.addRigidBody(newObjectID, geometry, posX, posY, posZ);
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
    return newObjectID++;
}

Playground::Objects::iterator Playground::removeObject(unsigned int id)
{
    /* 물리 데이터를 제거한다 */
    simulator.removePhysicsObject(id);
    /* 그래픽 데이터를 제거한다 */
    renderer.removeShape(id);

    Objects::iterator objectIter = objects.find(id);
    /* 선택된 오브젝트였다면 selectedObjectIDs 에서 제거한다 */
    if (objectIter->second->isSelected)
    {
        for (auto it = selectedObjectIDs.begin(); it != selectedObjectIDs.end(); ++it)
        {
            if (*it == objectIter->second->id)
            {
                selectedObjectIDs.erase(it);
                break;
            }
        }
    }
    /* 오브젝트를 objects 에서 제거하고 메모리에서 해제한다 */
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

    else if (typeid(*event) == typeid(RenderContactInfoFlagChangedEvent))
        handleRenderContactInfoFlagChangedEvent(static_cast<RenderContactInfoFlagChangedEvent*>(event));

    else if (typeid(*event) == typeid(AllObjectRemovedEvent))
        handleAllObjectRemovedEvent(static_cast<AllObjectRemovedEvent*>(event));

    else if (typeid(*event) == typeid(GroundRestitutionChangedEvent))
        handleGroundRestitutionChangedEvent(static_cast<GroundRestitutionChangedEvent*>(event));

    else if (typeid(*event) == typeid(ObjectRestitutionChangedEvent))
        handleObjectRestitutionChangedEvent(static_cast<ObjectRestitutionChangedEvent*>(event));

    else if (typeid(*event) == typeid(GravityChangedEvent))
        handleGravityChangedEvent(static_cast<GravityChangedEvent*>(event));

    else if (typeid(*event) == typeid(ObjectRotatedEvent))
        handleObjectRotatedEvent(static_cast<ObjectRotatedEvent*>(event));

    else if (typeid(*event) == typeid(OrientationResetEvent))
        handleOrientationResetEvent(static_cast<OrientationResetEvent*>(event));

    else if (typeid(*event) == typeid(ShouldRenderWorldAxis))
        handleShouldRenderWorldAxis(static_cast<ShouldRenderWorldAxis*>(event));

    else if (typeid(*event) == typeid(RemoveUnfixedObjectsEvent))
        handleRemoveUnfixedObjectsEvent(static_cast<RemoveUnfixedObjectsEvent*>(event));

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
    static bool isSpaceRepeated = false;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (!isSpaceRepeated)
        {
            isSimulating = !isSimulating;
            isSpaceRepeated = true;
        }
    }
    else if (glfwGetKey(renderer.getWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE)
        isSpaceRepeated = false;

    /* 프리셋 불러오기 */
    static bool isOneRepeated = false;
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_F1) == GLFW_PRESS)
    {
        if (!isOneRepeated)
        {
            loadPreset1();
            isOneRepeated = true;
        }
    }
    else if (glfwGetKey(renderer.getWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE)
        isOneRepeated = false;
}

void Playground::clearSelectedObjectIDs()
{
    for (const auto& id : selectedObjectIDs)
        objects.find(id)->second->isSelected = false;
    
    selectedObjectIDs.clear();
}

void Playground::loadPreset1()
{
    isSimulating = false;
    handleAllObjectRemovedEvent(nullptr);

    unsigned int sphereID = addObject(SPHERE, 0.0f, 1.0f, 7.0f);
    addObject(BOX, 0.0f, 0.5f, 0.0f);
    addObject(BOX, 1.2f, 0.5f, 0.0f);
    addObject(BOX, -1.2f, 0.5f, 0.0f);
    addObject(BOX, 0.7f, 1.5f, 0.0f);
    addObject(BOX, -0.7f, 1.5f, 0.0f);
    addObject(BOX, 0.0f, 2.5f, 0.0f);

    objects.find(sphereID)->second->body->setVelocity(0.0f, 0.0f, -30.0f);
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
    physics::RigidBody* body = objects.find(event->id)->second->body;
    body->setPosition(position[0], position[1], position[2]);
    body->setVelocity(0.0f, 0.0f, 0.0f);
    body->setRotation(0.0f, 0.0f, 0.0f);
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
    renderer.moveCamera(glm::vec3(0.0f, 0.0f, event->value * 10.0f));
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
    if (event->shouldBeFixed)
    {
        target->isFixed = true;
        target->body->setInverseMass(0.0f);
        target->body->setInverseInertiaTensor(physics::Matrix3(0.0f));
        target->body->setVelocity(0.0f, 0.0f, 0.0f);
        target->body->setRotation(0.0f, 0.0f, 0.0f);
    }
    else
    {
        target->isFixed = false;
        target->body->setMass(5.0f);

        physics::Matrix3 inertiaTensor;
        float geometricData[3] = {0};
        target->getGeometricDataInArray(geometricData);
        if (typeid(*target) == typeid(SphereObject))
        {
            SphereObject* sphere = static_cast<SphereObject*>(target);
            inertiaTensor.setDiagonal(0.4f * sphere->body->getMass() * geometricData[0]*geometricData[0]);
        }
        else if (typeid(*target) == typeid(BoxObject))
        {
            BoxObject* box = static_cast<BoxObject*>(target);
            float k = box->body->getMass() / 12;
            float x = geometricData[0] * 2.0f;
            float y = geometricData[1] * 2.0f;
            float z = geometricData[2] * 2.0f;
            inertiaTensor.entries[0] = k * (y*y + z*z);
            inertiaTensor.entries[4] = k * (x*x + z*z);
            inertiaTensor.entries[8] = k * (y*y + x*x);
        }
        target->body->setInertiaTensor(inertiaTensor);
    }
}

void Playground::handleRenderContactInfoFlagChangedEvent(RenderContactInfoFlagChangedEvent* event)
{
    shouldRenderContactInfo = event->flag;
}

void Playground::handleAllObjectRemovedEvent(AllObjectRemovedEvent* event)
{
    Objects::iterator iter = objects.begin();
    while (iter != objects.end())
    {
        iter = removeObject(iter->first);
    }
    selectedObjectIDs.clear();
}

void Playground::handleGroundRestitutionChangedEvent(GroundRestitutionChangedEvent* event)
{
    simulator.setGroundRestitution(event->value);
}

void Playground::handleObjectRestitutionChangedEvent(ObjectRestitutionChangedEvent* event)
{
    simulator.setObjectRestitution(event->value);
}

void Playground::handleGravityChangedEvent(GravityChangedEvent* event)
{
    simulator.setGravity(event->value);
}

void Playground::handleObjectRotatedEvent(ObjectRotatedEvent* event)
{
    Object* target = objects.find(event->id)->second;
    /* 회전각을 라디안으로 변환한다 */
    float radian = event->degree * PI / 180.0f;
    /* 회전축을 오브젝트의 로컬 좌표계로 변환한다 */
    physics::Matrix4 transformMat = target->body->getTransformMatrix();
    physics::Matrix3 rotationMat;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            rotationMat.entries[3*i + j] = transformMat.entries[4*i + j];

    physics::Vector3 axisWorld(event->axisX, event->axisY, event->axisZ);
    physics::Vector3 axisLocal = rotationMat.transpose() * axisWorld;
    axisLocal.normalize();
    /* 앞서 계산한 회전각과 축을 토대로 회전 사원수를 계산한다 */
    physics::Quaternion quat(
        cosf(radian * 0.5f),
        sinf(radian * 0.5f) * axisLocal.x,
        sinf(radian * 0.5f) * axisLocal.y,
        sinf(radian * 0.5f) * axisLocal.z
    );
    target->body->rotateByQuat(quat);
}

void Playground::handleOrientationResetEvent(OrientationResetEvent* event)
{
    Object* target = objects.find(event->id)->second;
    target->body->setOrientation(physics::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
}

void Playground::handleShouldRenderWorldAxis(ShouldRenderWorldAxis* event)
{
    Object* target = objects.find(event->id)->second;
    physics::Vector3 pos = target->body->getPosition();
    renderer.renderWorldAxisAt(event->axisIdx, pos.x, pos.y, pos.z);
}

void Playground::handleRemoveUnfixedObjectsEvent(RemoveUnfixedObjectsEvent* event)
{
    Objects::iterator iter = objects.begin();
    while (iter != objects.end())
    {
        if (!iter->second->isFixed)
            iter = removeObject(iter->second->id);
        else
            ++iter;
    }
}
