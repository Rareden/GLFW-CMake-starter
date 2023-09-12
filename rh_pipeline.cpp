//
// Created by Isaac on 26-Aug-23.
//
#include "rh_pipeline.h"
#include <fstream>
#include <iostream>
#include <assert.h>
#include "Windows.h"


using namespace std;


RhPipeline::RhPipeline(lve::MyEngineDevice& EngineDevice, const std::string& VertFilePath, const std::string& FragFilePath, const PipelineConfigInfo& ConfigInfo) : EngineDevice(EngineDevice)
{
    CreateGraphicsPipeline(VertFilePath,FragFilePath,ConfigInfo);

}

std::vector<char> RhPipeline::ReadFile(const std::string &FilePath)
{
    const unsigned long maxDir = 260;
    char currentDir[maxDir];
    GetCurrentDirectory(maxDir,currentDir);
    std::cout << currentDir <<"\n";

    std::ifstream File{FilePath, std::ios::ate | std::ios::binary};

    if(!File.is_open())
    {
        throw std::runtime_error("Failed to open File: "+ FilePath);
    }

    size_t FileSize = static_cast<size_t>(File.tellg());

    std::vector<char> buffer(FileSize);

    File.seekg(0);
    File.read(buffer.data(),FileSize);
    File.close();

    return buffer;
}

void RhPipeline::CreateGraphicsPipeline(const std::string& VertFilePath, const std::string& FragFilePath,const PipelineConfigInfo& ConfigInfo)
{
    assert(ConfigInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create Graphics Pipeline: no pipelineLayout provided in configInfo");
    assert(ConfigInfo.renderPass != VK_NULL_HANDLE && "Cannot create Graphics Pipeline: no renderPass provided in configInfo");
    auto VertCode = ReadFile(VertFilePath);
    auto FragCode = ReadFile(FragFilePath);
    CreateShadermodule(VertCode,&vertShaderModule);
    CreateShadermodule(FragCode,&fragShaderModule);



    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    vertShaderStageInfo.flags = 0;
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    fragShaderStageInfo.flags = 0;
    VkPipelineShaderStageCreateInfo ShaderStageInfo[]= {vertShaderStageInfo,fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineViewportStateCreateInfo ViewportInfo{};
    ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportInfo.viewportCount = 1;
    ViewportInfo.pViewports = &ConfigInfo.viewport;
    ViewportInfo.scissorCount = 1;
    ViewportInfo.pScissors = &ConfigInfo.scissor;

    VkGraphicsPipelineCreateInfo PipelineInfo{};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineInfo.stageCount = 2;
    PipelineInfo.pStages = ShaderStageInfo;
    PipelineInfo.pVertexInputState = &vertexInputInfo;
    PipelineInfo.pInputAssemblyState = &ConfigInfo.InputAssemblyInfo;
    PipelineInfo.pViewportState = &ViewportInfo;
    PipelineInfo.pRasterizationState = &ConfigInfo.rasterizationInfo;
    PipelineInfo.pMultisampleState = &ConfigInfo.multisampleInfo;
    PipelineInfo.pColorBlendState = &ConfigInfo.colorBlendInfo;
    PipelineInfo.pDepthStencilState = &ConfigInfo.depthStencilInfo;

    PipelineInfo.pDynamicState = nullptr;
    PipelineInfo.layout = ConfigInfo.pipelineLayout;
    PipelineInfo.renderPass = ConfigInfo.renderPass;
    PipelineInfo.subpass = ConfigInfo.subpass;
    PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    PipelineInfo.basePipelineIndex = -1;

    assert(EngineDevice.device() != VK_NULL_HANDLE && "Cannot create Graphics Pipeline: no device provided in configInfo"  );


    if(vkCreateGraphicsPipelines(EngineDevice.device(),VK_NULL_HANDLE,1,&PipelineInfo, nullptr,&graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Graphics Pipeline");
    }

    vkDestroyShaderModule(EngineDevice.device(),vertShaderModule, nullptr);
    vkDestroyShaderModule(EngineDevice.device(),fragShaderModule, nullptr);
}

PipelineConfigInfo RhPipeline::DefaultPipelineConfigInfo(uint32_t width, uint32_t height) {
    PipelineConfigInfo Info{};
    Info.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    Info.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    Info.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    Info.viewport.x = 0.0f;
    Info.viewport.y = 0.0f;
    Info.viewport.width = static_cast<float>(width);
    Info.viewport.height = static_cast<float>(height);
    Info.viewport.minDepth = 0.0f;
    Info.viewport.maxDepth = 1.0f;

    Info.scissor.offset = {0, 0};
    Info.scissor.extent = {width, height};
    Info.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Info.rasterizationInfo.depthClampEnable = VK_FALSE;
    Info.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    Info.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    Info.rasterizationInfo.lineWidth = 1.0f;
    Info.rasterizationInfo.cullMode = VK_CULL_MODE_NONE; // todo cull back faces
    Info.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    Info.rasterizationInfo.depthBiasEnable = VK_FALSE;
    Info.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
    Info.rasterizationInfo.depthBiasClamp = 0.0f;          // Optional
    Info.rasterizationInfo.depthBiasSlopeFactor = 0.0f;    // Optional
    Info.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Info.multisampleInfo.sampleShadingEnable = VK_FALSE;
    Info.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    Info.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    Info.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    Info.colorBlendInfo.attachmentCount=1;

    Info.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
    Info.colorBlendAttachment.blendEnable = VK_FALSE;
    Info.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    Info.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    Info.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
    Info.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    Info.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    Info.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional
    Info.colorBlendInfo.pAttachments = &Info.colorBlendAttachment;



    return Info;
}

void RhPipeline::CreateShadermodule(const vector<char> &Code, VkShaderModule *ShaderModule) {

    VkShaderModuleCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    CreateInfo.codeSize = Code.size() * sizeof(char);
    CreateInfo.pCode = reinterpret_cast<const uint32_t*>(Code.data());
    if(vkCreateShaderModule(EngineDevice.device(),&CreateInfo, nullptr,ShaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Shader Module");
    }
}

RhPipeline::~RhPipeline() {
    vkDestroyShaderModule(EngineDevice.device(),vertShaderModule, nullptr);
    vkDestroyShaderModule(EngineDevice.device(),fragShaderModule, nullptr);
    vkDestroyPipeline(EngineDevice.device(),graphicsPipeline, nullptr);
}


