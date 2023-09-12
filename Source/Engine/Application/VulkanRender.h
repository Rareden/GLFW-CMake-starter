//
// Created by Isaac on 27-Aug-23.
//
#pragma once
#ifndef GLFW_CMAKE_STARTER_VULKANRENDER_H
#define GLFW_CMAKE_STARTER_VULKANRENDER_H

#include <iostream>
#include <chrono>
#include <thread>
#define GLFW_INCLUDE_VULKAN
#include "AppWindow.h"
#include "../../../rh_pipeline.h"
#include "SwapChain.hpp"

namespace Engine {


    class VulkanRenderer {
    public:
        // remove copy constructors
        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(const VulkanRenderer&) = delete;

        VulkanRenderer(){
            RenderInterface= nullptr;
            window = nullptr;
            EngineDevice = nullptr;
            pipelineLayout= VK_NULL_HANDLE;
        }
        void initVulkan() {
	        CreatePipelineLayout();
            CreatePipeline();
            CreateCommandBuffers();

        }
        void createWindow();

        void mainLoop() ;

        void cleanup() ;
        void BindCommandBuffer(VkCommandBuffer commandBuffer);

    private:
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void DrawFrame();


        DatWindow* window;
        bool shouldClose = false;
        RhPipeline* RenderInterface;
        lve::MyEngineDevice* EngineDevice;
        std::unique_ptr<lve::MyEngineSwapChain> SwapChain;
        VkPipelineLayout  pipelineLayout{};
        std::vector<VkCommandBuffer> commandBuffers;
        uint64_t FrameCounter;
    };

} // Engine

#endif //GLFW_CMAKE_STARTER_VULKANRENDER_H
