//
// Created by Isaac on 26-Aug-23.
//
#pragma once
#ifndef GLFW_CMAKE_STARTER_APPWINDOW_H
#define GLFW_CMAKE_STARTER_APPWINDOW_H

#endif //GLFW_CMAKE_STARTER_APPWINDOW_H
#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



class DatWindow {
public:

    DatWindow(int Width, int Height, std::string WindowName);
    ~DatWindow();
    /* non copyable */
    DatWindow(const DatWindow& ) = delete;
    DatWindow &operator = (const DatWindow& ) = delete;
    VkExtent2D GetWindowSize() const{
        int width, height;
        // determine size of the window
        glfwGetWindowSize(Window, &width, &height);
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }

    bool ShouldClose( )
    {
        if(Window)
        {
            return glfwWindowShouldClose(Window);
        }
        return true;
    }
    void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    void TickWindow()
    {
        if(!Window)
        {
            return;
        }
        /* Swap front and back buffers */
        glfwSwapBuffers(Window);
    }
    GLFWwindow* GetWindow() const { return Window; }
private:
    GLFWwindow* Window{};

};
