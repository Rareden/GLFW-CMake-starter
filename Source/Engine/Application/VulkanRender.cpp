//
// Created by Isaac on 27-Aug-23.
//

#include <array>
#include <assert.h>
#include "VulkanRender.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
static uint32_t                 g_QueueFamily = (uint32_t)-1;

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

namespace Engine {
	void VulkanRenderer::createWindow()
	{
        FrameCounter=0;
		/* Create a windowed mode window and its OpenGL context */
		window = new DatWindow(800, 600, std::string("Vulkan Window"));
        EngineDevice = new lve::MyEngineDevice(*window);

		std::cout << "Render interface created" << std::endl;

		if (! window)
		{
			glfwTerminate();
			return;
		}
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForVulkan(window->GetWindow(), true);
        //ImGui::ShowDemoWindow();
	}

	void VulkanRenderer::mainLoop()
	{

		/* Loop until the user closes the window */
		while (! window->ShouldClose())
		{
            auto currentTime = std::chrono::high_resolution_clock::now();

			window->TickWindow();

			/* Poll for and process events */
			glfwPollEvents();
            DrawFrame();
            FrameCounter++;
			auto NewTime = std::chrono::high_resolution_clock::now();

            //std::cout << "Frame: " << FrameCounter << " Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(NewTime - currentTime).count()<<" ms" << std::endl;
			// sleep thread for 1ms
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	void VulkanRenderer::cleanup()
	{

		// TODO: Clean up Vulkan resources
		SwapChain.reset();
		vkDestroyPipelineLayout(EngineDevice->device(), pipelineLayout, nullptr);

		delete RenderInterface;
		delete EngineDevice;
		delete window;
        glfwTerminate();

	}

	void VulkanRenderer::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
		if (vkCreatePipelineLayout(EngineDevice->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void VulkanRenderer::CreatePipeline()
	{
        assert(EngineDevice != nullptr && "Cannot create pipeline before device");

        SwapChain = std::make_unique<lve::MyEngineSwapChain>(*EngineDevice, window->GetWindowSize());


        auto PipelineConfig = RhPipeline::DefaultPipelineConfigInfo(SwapChain->width(), SwapChain->height());
		PipelineConfig.renderPass = SwapChain->GetRenderPass();
        PipelineConfig.pipelineLayout = pipelineLayout;
        assert(PipelineConfig.renderPass != nullptr && "Cannot create pipeline before render pass");

		const std::string Path1 = R"(C:\Users\Isaac\source\repos\DatEngine\Shaders\simple_shader.vert.spv)";
		const std::string Path2 = R"(C:\Users\Isaac\source\repos\DatEngine\Shaders\simple_shader.frag.spv)";
		RenderInterface = new RhPipeline(*EngineDevice, Path1, Path2, PipelineConfig);

	}

	void VulkanRenderer::CreateCommandBuffers()
	{
        commandBuffers.resize(SwapChain->imageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = EngineDevice->GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if(vkAllocateCommandBuffers(EngineDevice->device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers");
        }
        int index = 0;
        for(auto & commandBuffer : commandBuffers)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to begin recording command buffer");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = SwapChain->GetRenderPass();
            renderPassInfo.framebuffer = SwapChain->GetFrameBuffer(index);

            renderPassInfo.renderArea.offset = {0,0};
            renderPassInfo.renderArea.extent = SwapChain->getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[index], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            BindCommandBuffer(commandBuffers[index]);
            vkCmdDraw(commandBuffers[index], 3, 1, 0, 0);
            vkCmdEndRenderPass(commandBuffers[index]);
            if(vkEndCommandBuffer(commandBuffers[index]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to record command buffer");
            }
            index++;
        }
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = EngineDevice->GetInstance();
        init_info.PhysicalDevice = EngineDevice->GetPhysicalDevice();
        init_info.Device = EngineDevice->GetDevice();
        init_info.QueueFamily = g_QueueFamily;
        init_info.Queue = EngineDevice->GetGraphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = EngineDevice->GetDescriptorPool();
        init_info.Subpass = 0;
        init_info.MinImageCount = 2;
        init_info.ImageCount = SwapChain->imageCount();
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, SwapChain->GetRenderPass());
	}

	void VulkanRenderer::DrawFrame()
	{
        uint32_t imageIndex;
        auto result = SwapChain->acquireNextImage(&imageIndex);
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            //RecreateSwapChain();
            throw std::runtime_error("Failed to acquire swap chain image");
            return;
        }
        result = SwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if(result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swap chain image");
        }
	}

    void VulkanRenderer::BindCommandBuffer(VkCommandBuffer commandBuffer)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, RenderInterface->GetPipeline());
    }
} // Engine