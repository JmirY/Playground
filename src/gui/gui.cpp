#include <gui/gui.h>
#include <GLFW/glfw3.h>

using namespace gui;

GUI::GUI(GLFWwindow* window, unsigned int _textureBufferID)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    textureBufferID = _textureBufferID;
}

void GUI::renderAll()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoTitleBar;

    renderScene(windowFlags);
    renderObjectPalette(windowFlags);
    renderInspector(windowFlags);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}

void GUI::renderScene(ImGuiWindowFlags windowFlags)
{
    
    ImGui::Begin("Scene", NULL, windowFlags);
    {
        ImGui::BeginChild("SceneRender");
        ImGui::Image(
            (ImTextureID)(intptr_t) textureBufferID,
            ImGui::GetWindowSize(),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderObjectPalette(ImGuiWindowFlags windowFlags)
{    
    ImGui::Begin("ObjectPalette", NULL, windowFlags);
    {
        ImGui::BeginChild("ObjectPaletteRender");
        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderInspector(ImGuiWindowFlags windowFlags)
{    
    ImGui::Begin("Inspector", NULL, windowFlags);
    {
        ImGui::BeginChild("InspectorRender");
        ImGui::EndChild();
    }
    ImGui::End();
}
