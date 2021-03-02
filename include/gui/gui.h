#ifndef GUI_H
#define GUI_H

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../playground/event_queue.h"

namespace gui
{
    class GUI
    {
    private:
        unsigned int textureBufferID;

    public:
        GUI() {}
        GUI(GLFWwindow* window, unsigned int textureBufferID);

        void renderAll(EventQueue&);

    private:
        void renderScene(ImGuiWindowFlags, EventQueue&);
        void renderObjectPalette(ImGuiWindowFlags, EventQueue&);
        void renderInspector(ImGuiWindowFlags, EventQueue&);
    };
} // namespace gui


#endif // GUI_H
