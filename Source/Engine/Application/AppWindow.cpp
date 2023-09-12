//
// Created by Isaac on 26-Aug-23.
//
#include "AppWindow.h"

#include <stdexcept>

DatWindow::DatWindow(int Width, int Height, std::string WindowName) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    Window = glfwCreateWindow(Width, Height, WindowName.c_str(), nullptr, nullptr);
}

DatWindow::~DatWindow()
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}

void DatWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {

    if (glfwCreateWindowSurface(instance, Window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

