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

    renderScene(windowFlags, eventQueue);
    renderObjectPalette(windowFlags, eventQueue);
    renderSimulationControl(windowFlags, eventQueue, isSimulating);
    renderObjectList(windowFlags, eventQueue, objects);
    renderObjectAttribute(windowFlags, eventQueue, objects, selectedObjectIDs);

    ImGui::ShowDemoWindow(); // test

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}

void GUI::renderScene(ImGuiWindowFlags windowFlags, EventQueue& eventQueue)
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1024, 576));
    ImGui::Begin("Scene", NULL, windowFlags);
    {
        ImGui::BeginChild("SceneRender");
        ImGui::Image(
            (ImTextureID)(uintptr_t) textureBufferID,
            ImGui::GetWindowSize(),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
        ImGui::EndChild();
    }
    ImGui::End();
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
    const std::vector<unsigned int>& selectedObjectIDs
)
{
    ImGui::SetNextWindowPos(ImVec2(1024, 286));
    ImGui::SetNextWindowSize(ImVec2(256, 434));
    ImGui::Begin("ObjectAttribute", NULL, windowFlags);
    {
        ImGui::BeginChild("ObjectAttributeRender");

        if (selectedObjectIDs.size() == 1)
        {
            ImGui::Columns(4);
            ImGui::NextColumn(); ImGui::AlignTextToFramePadding();
            ImGui::Text("X"); ImGui::NextColumn(); ImGui::AlignTextToFramePadding();
            ImGui::Text("Y"); ImGui::NextColumn(); ImGui::AlignTextToFramePadding();
            ImGui::Text("Z"); ImGui::NextColumn();
            ImGui::Separator();

            const Object* object = objects.find(selectedObjectIDs[0])->second;
            float vecBuffer[3];
            /* 위치 */
            object->getPositionInArray(vecBuffer);
            ImGui::Text("Position"); ImGui::NextColumn();
            if (ImGui::DragFloat("##PositionX", &vecBuffer[0], 0.1f))
                eventQueue.push(new ObjectPositionChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            if (ImGui::DragFloat("##PositionY", &vecBuffer[1], 0.1f, 0.0f, FLT_MAX))
                eventQueue.push(new ObjectPositionChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            if (ImGui::DragFloat("##PositionZ", &vecBuffer[2], 0.1f))
                eventQueue.push(new ObjectPositionChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::NextColumn();
            /* 속도 */
            object->getVelocityInArray(vecBuffer);
            ImGui::Text("Velocity"); ImGui::NextColumn();
            ImGui::DragFloat("##VelocityX", &vecBuffer[0], 0.1f);
            ImGui::NextColumn();
            ImGui::DragFloat("##VelocityY", &vecBuffer[1], 0.1f);
            ImGui::NextColumn();
            ImGui::DragFloat("##VelocityZ", &vecBuffer[2], 0.1f);
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
            ImGui::DragFloat("##Mass", &vecBuffer[0], 0.1f);
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
