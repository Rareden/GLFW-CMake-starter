#pragma once
//
// Created by Isaac on 26-Aug-23.
//

#ifndef GLFW_CMAKE_STARTER_RH_PIPELINE_H
#define GLFW_CMAKE_STARTER_RH_PIPELINE_H

#endif //GLFW_CMAKE_STARTER_RH_PIPELINE_H
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "Dat_Engine_Device.hpp"

struct PipelineConfigInfo {
    VkViewport viewport{};
    VkRect2D scissor{};
    VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;

};
class RhPipeline {

public:

    RhPipeline(lve::MyEngineDevice& EngineDevice, const std::string& VertFilePath, const std::string& FragFilePath, const PipelineConfigInfo& ConfigInfo);
    ~RhPipeline();

    RhPipeline(const RhPipeline&) = delete;
    void operator=(const RhPipeline&) = delete;

    static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);

    [[nodiscard]] VkPipeline GetPipeline() const { return graphicsPipeline; }

private:

    static std::vector<char> ReadFile(const std::string &FilePath);
    void CreateGraphicsPipeline(const std::string& VertFilePath, const std::string& FragFilePath,const PipelineConfigInfo& ConfigInfo);


    void CreateShadermodule(const std::vector<char>& Code, VkShaderModule* ShaderModule);

    lve::MyEngineDevice& EngineDevice;
    VkPipeline graphicsPipeline{};
    VkShaderModule vertShaderModule{};
    VkShaderModule fragShaderModule{};

};


