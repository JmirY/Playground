#include <gui/gui.h>
#include <GLFW/glfw3.h>
#include <playground/geometry.h>
#include <string>
#include <cfloat>

using namespace gui;

GUI::GUI(GLFWwindow* window, unsigned int _textureBufferID)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    textureBufferID = _textureBufferID;

    /* imgui 전역 스타일 설정 */
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
}

void GUI::renderAll(
    EventQueue& eventQueue,
    const Objects& objects,
    const bool& isSimulating,
    const std::vector<unsigned int>& selectedObjectIDs
)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoTitleBar;

    renderScene(windowFlags, eventQueue, isSimulating);
    renderObjectPalette(windowFlags, eventQueue);
    renderSimulationControl(windowFlags, eventQueue, isSimulating);
    renderObjectList(windowFlags, eventQueue, objects);
    renderObjectAttribute(windowFlags, eventQueue, objects, selectedObjectIDs, isSimulating);

    ImGui::ShowDemoWindow(); // test

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}

void GUI::renderScene(
    ImGuiWindowFlags windowFlags,
    EventQueue& eventQueue,
    const bool& isSimulating
)
{
    if (!isSimulating)
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1024, 576));
    ImGui::Begin("Scene", NULL, windowFlags);
    {
        ImGui::BeginChild("SceneRender");

        /* Scene 렌더 */
        ImGui::Image(
            (ImTextureID)(uintptr_t) textureBufferID,
            ImGui::GetWindowSize(),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        const ImGuiIO& io = ImGui::GetIO();
        /* 왼쪽 마우스 드래그 */
        ImVec2 clickedPos = io.MouseClickedPos[0];
        if (isInScene(clickedPos))
        {
            if (ImGui::IsMouseDragging(0))
            {
                ImVec2 dragDelta = io.MouseDelta;
                if (dragDelta.x != 0.0f && dragDelta.y != 0.0f)
                    eventQueue.push(new LeftMouseDraggedOnSceneEvent(dragDelta.x, -dragDelta.y));
            }
            else if (ImGui::IsMouseReleased(0) && io.MouseDownDurationPrev[0] < 0.03f)
            {
                eventQueue.push(new LeftMouseClickedOnSceneEvent(clickedPos.x, clickedPos.y, io.KeyCtrl));
            }
        }

        /* 오른쪽 마우스 드래그 */
        clickedPos = io.MouseClickedPos[1];
        if (isInScene(clickedPos) && ImGui::IsMouseDragging(1))
        {
            ImVec2 dragDelta = io.MouseDelta;
            if (dragDelta.x != 0.0f && dragDelta.y != 0.0f)
            {
                eventQueue.push(new RightMouseDraggedOnSceneEvent(
                    clickedPos.x + dragDelta.x * 10.0f,
                    clickedPos.y + dragDelta.y * 10.0f,
                    clickedPos.x,
                    clickedPos.y
                ));
            }
        }
        /* 마우스 휠 스크롤 */
        ImVec2 mousePos = ImGui::GetMousePos();
        if (isInScene(mousePos))
        {
            float mouseWheel = io.MouseWheel;
            if (mouseWheel != 0.0f)
                eventQueue.push(new MouseWheelOnSceneEvent(mouseWheel));
        }

        ImGui::EndChild();
    }
    ImGui::End();

    if (!isSimulating)
        ImGui::PopStyleColor();
}

void GUI::renderObjectPalette(ImGuiWindowFlags windowFlags, EventQueue& eventQueue)
{    
    ImGui::SetNextWindowPos(ImVec2(0, 576));
    ImGui::SetNextWindowSize(ImVec2(1024, 144));
    ImGui::Begin("ObjectPalette", NULL, windowFlags);
    {
        ImGui::BeginChild("ObjectPaletteRender");

        /* 오브젝트 추가 버튼을 렌더한다 */
        ImVec2 buttonSize(100, 100);
        if (ImGui::Button("Sphere", buttonSize))
        {
            eventQueue.push(new ObjectAddedEvent(SPHERE));
        }
        ImGui::SameLine();
        if (ImGui::Button("Box", buttonSize))
        {
            eventQueue.push(new ObjectAddedEvent(BOX));
        }
        
        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderSimulationControl(
    ImGuiWindowFlags windowFlags,
    EventQueue& eventQueue,
    const bool& isSimulating
)
{
    ImGui::SetNextWindowPos(ImVec2(1024, 0));
    ImGui::SetNextWindowSize(ImVec2(256, 70));
    ImGui::Begin("SimulationControl", NULL, windowFlags);
    {
        ImGui::BeginChild("SimulationControlRender");

        /* 시뮬레이션 멈춤/재개 버튼 */
        std::string label("Pause Simulation");
        if (!isSimulating)
            label = "Resume Simulation";
        if (ImGui::Button(label.c_str()))
            eventQueue.push(new SimulationStatusChangedEvent);

        /* 오브젝트 삭제 버튼 */
        if (ImGui::Button("Remove Selected Objects"))
            eventQueue.push(new ObjectRemovedEvent);

        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderObjectList(
    ImGuiWindowFlags windowFlags,
    EventQueue& eventQueue,
    const Objects& objects
)
{
    ImGui::SetNextWindowPos(ImVec2(1024, 70));
    ImGui::SetNextWindowSize(ImVec2(256, 216));
    ImGui::Begin("ObjectList", NULL, windowFlags);
    {
        ImGui::BeginChild("ObjectListRender");

        /* 생성된 오브젝트들을 나열한다 */
        int i = 1;
        for (const auto& object : objects)
        {
            ImGui::PushID(i);

            if (ImGui::Selectable("Object", object.second->getIsSelected(), 0, ImVec2(50, 50)))
            {
                /* ctrl 키를 누른 채로 클릭하면 다중 선택이 가능하다 */
                if (ImGui::GetIO().KeyCtrl)
                    eventQueue.push(new ObjectSelectedEvent(object.second->getID(), true));
                else
                    eventQueue.push(new ObjectSelectedEvent(object.second->getID(), false));
            }

            if (i % 4 != 0)
                ImGui::SameLine();
            ++i;
            ImGui::PopID();
        }

        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderObjectAttribute(
    ImGuiWindowFlags windowFlags,
    EventQueue& eventQueue,
    const Objects& objects,
    const std::vector<unsigned int>& selectedObjectIDs,
    const bool& isSimulating
)
{
    ImGui::SetNextWindowPos(ImVec2(1024, 286));
    ImGui::SetNextWindowSize(ImVec2(256, 434));
    ImGui::Begin("ObjectAttribute", NULL, windowFlags);
    {
        ImGui::BeginChild("ObjectAttributeRender");

        if (selectedObjectIDs.size() == 1)
        {
            const Object* object = objects.find(selectedObjectIDs[0])->second;
            bool isObjectFixed = object->getIsFixed();

            ImGui::Columns(4);
            if (ImGui::Checkbox("Fixed", &isObjectFixed))
                eventQueue.push(new ObjectPositionFixedEvent(selectedObjectIDs[0], isObjectFixed));
            ImGui::NextColumn(); ImGui::AlignTextToFramePadding();
            ImGui::Text("X"); ImGui::NextColumn(); ImGui::AlignTextToFramePadding();
            ImGui::Text("Y"); ImGui::NextColumn(); ImGui::AlignTextToFramePadding();
            ImGui::Text("Z"); ImGui::NextColumn();
            ImGui::Separator();

            float vecBuffer[3];
            /* 위치 */
            object->getPositionInArray(vecBuffer);
            ImGui::Text("Position"); ImGui::NextColumn();
            if (ImGui::DragFloat("##PositionX", &vecBuffer[0], 0.05f))
                eventQueue.push(new ObjectPositionChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            if (ImGui::DragFloat("##PositionY", &vecBuffer[1], 0.05f, 0.0f, FLT_MAX))
                eventQueue.push(new ObjectPositionChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            if (ImGui::DragFloat("##PositionZ", &vecBuffer[2], 0.05f))
                eventQueue.push(new ObjectPositionChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            /* 속도 */
            object->getVelocityInArray(vecBuffer);
            ImGui::Text("Velocity"); ImGui::NextColumn();
            if (ImGui::DragFloat("##VelocityX", &vecBuffer[0], 0.1f))
                if (!isSimulating)
                    eventQueue.push(new ObjectVelocityChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            if (ImGui::DragFloat("##VelocityY", &vecBuffer[1], 0.1f))
                if (!isSimulating)
                    eventQueue.push(new ObjectVelocityChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            if (ImGui::DragFloat("##VelocityZ", &vecBuffer[2], 0.1f))
                if (!isSimulating)
                    eventQueue.push(new ObjectVelocityChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            /* 각속도 */
            object->getRotationInArray(vecBuffer);
            ImGui::Text("Rotation"); ImGui::NextColumn();
            ImGui::DragFloat("##RotationX", &vecBuffer[0], 0.1f);
                // if (!isSimulating)
                //     eventQueue.push(new ObjectVelocityChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            ImGui::DragFloat("##RotationY", &vecBuffer[1], 0.1f);
                // if (!isSimulating)
                //     eventQueue.push(new ObjectVelocityChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            ImGui::DragFloat("##RotationZ", &vecBuffer[2], 0.1f);
                // if (!isSimulating)
                //     eventQueue.push(new ObjectVelocityChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            /* 가속도 */
            object->getAccelerationInArray(vecBuffer);
            ImGui::Text("Acceleration"); ImGui::NextColumn();
            ImGui::DragFloat("##AccelerationX", &vecBuffer[0], 0.1f);
            ImGui::NextColumn();
            ImGui::DragFloat("##AccelerationY", &vecBuffer[1], 0.1f);
            ImGui::NextColumn();
            ImGui::DragFloat("##AccelerationZ", &vecBuffer[2], 0.1f);
            ImGui::Columns(1); ImGui::Separator(); ImGui::Spacing();
            /* 질량 */
            object->getMassInArray(vecBuffer);
            ImGui::Text("Mass");
            if (ImGui::DragFloat("##Mass", &vecBuffer[0], 0.1f, 0.1f, FLT_MAX))
                eventQueue.push(new ObjectMassChangedEvent(selectedObjectIDs[0], vecBuffer[0]));
            /* 도형 데이터 */
            object->getGeometricDataInArray(vecBuffer);
            Geometry geometry = object->getGeometry();
            if (geometry == BOX)
            {
                ImGui::Text("Half-size");
                ImGui::Text("X"); ImGui::SameLine();
                if (ImGui::DragFloat("##Half-X", &vecBuffer[0], 0.01f, 0.1f, FLT_MAX))
                    eventQueue.push(new ObjectGeometricDataChangedEvent(selectedObjectIDs[0], vecBuffer));
                ImGui::Text("Y"); ImGui::SameLine();
                if (ImGui::DragFloat("##Half-Y", &vecBuffer[1], 0.01f, 0.1f, FLT_MAX))
                    eventQueue.push(new ObjectGeometricDataChangedEvent(selectedObjectIDs[0], vecBuffer));
                ImGui::Text("Z"); ImGui::SameLine();
                if (ImGui::DragFloat("##Half-Z", &vecBuffer[2], 0.01f, 0.1f, FLT_MAX))
                    eventQueue.push(new ObjectGeometricDataChangedEvent(selectedObjectIDs[0], vecBuffer));
            }
            else if (geometry == SPHERE)
            {
                ImGui::Text("Radius");
                if (ImGui::DragFloat("##Radius", &vecBuffer[0], 0.01f, 0.1f, FLT_MAX))
                    eventQueue.push(new ObjectGeometricDataChangedEvent(selectedObjectIDs[0], vecBuffer));
            }
        }

        ImGui::EndChild();
    }
    ImGui::End();
}

bool GUI::isInScene(ImVec2 vec)
{
    if (vec.x > 0.0f && vec.x < 1024.0f && vec.y > 0.0f && vec.y < 576.0f)
        return true;
    else
        return false;
}
