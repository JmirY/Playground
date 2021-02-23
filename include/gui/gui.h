#ifndef GUI_H
#define GUI_H

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace gui
{
    class GUI
    {
    public:
        GUI() {}
        GUI(GLFWwindow* window);

    };
} // namespace gui


#endif // GUI_H
