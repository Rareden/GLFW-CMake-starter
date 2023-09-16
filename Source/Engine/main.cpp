#include "DatEngine/ThirdParty/glfw/include/GLFW/glfw3.h"
#include <vulkan/vulkan.h>
#include <string>
#include <thread>
#include <iostream>
#include <chrono>
#include <memory>
#include "DatEngine/Source/Engine/Application/VulkanRender.h"
#include "rh_pipeline.h"
#include "Dat_Engine_Device.hpp"
#include "DatEngine/Source/Engine/Application/AppWindow.h"
#include "DatEngine/Source/Engine/Application/SwapChain.hpp"
#include "imgui.h"

// function ptr parameter
void f1( int(*func)(int) ){

}
int myFunction(int x) {
    return x * 2;
}

int main(void)
{
    Engine::VulkanRenderer app;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    //f1(myFunction);

    //int& (*fp1)(int*) =[](auto* a) -> auto& { return *a; };

    try {
        app.createWindow();
        app.initVulkan();
        app.mainLoop();
        app.cleanup();
        glfwTerminate();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }



    return EXIT_SUCCESS;
}
