#pragma once

#include "DatEngine/Source/Engine/Dat_Engine_Device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>

namespace lve {

    class MyEngineSwapChain
    {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        MyEngineSwapChain(MyEngineDevice &deviceRef, VkExtent2D windowExtent);

        ~MyEngineSwapChain();

        MyEngineSwapChain(const MyEngineSwapChain &) = delete;

        void operator=(const MyEngineSwapChain &) = delete;

        VkFramebuffer GetFrameBuffer(int index)
        { return swapChainFramebuffers[index]; }

        VkRenderPass GetRenderPass()
        { return renderPass; }

        VkImageView GetImageView(int index)
        { return swapChainImageViews[index]; }

        size_t imageCount()
        { return swapChainImages.size(); }

        VkFormat getSwapChainImageFormat()
        { return swapChainImageFormat; }

        VkExtent2D getSwapChainExtent()
        { return swapChainExtent; }

        uint32_t width()
        { return swapChainExtent.width; }

        uint32_t height()
        { return swapChainExtent.height; }

        float extentAspectRatio()
        {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }

        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t *imageIndex);

        VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

    private:
        void createSwapChain();

        void createImageViews();

        void createDepthResources();

        void createRenderPass();

        void CreateFrameBuffers();

        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(
                const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        MyEngineDevice &device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
    };

}  // namespace lve
