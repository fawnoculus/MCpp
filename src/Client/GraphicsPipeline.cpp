#include <limits>

#include "glslang/Public/ShaderLang.h"

#include "ResourceManager.h"
#include "GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline(const std::shared_ptr<spdlog::logger> &a_logger, const ResourceManager &resourceManager, const Utils::Identifier &a_identifier)
    : m_logger(a_logger)
{
    const auto fragmentSpirV = resourceManager.getCompiledShader(a_identifier.withSuffixedPath(".frag"), EShLangFragment);
    if (!fragmentSpirV.has_value())
    {
        throw std::runtime_error("Failed to create shader, shader '" + a_identifier.toString() + ".frag' Couldn't be compiled");
    }
    m_fragmentSpirV = fragmentSpirV.value();
    m_vkFragmentShaderCreateInfo.codeSize = m_fragmentSpirV.size() * sizeof(uint32_t);
    m_vkFragmentShaderCreateInfo.pCode = m_fragmentSpirV.data();

    const auto vertexSpirV = resourceManager.getCompiledShader(a_identifier.withSuffixedPath(".vert"), EShLangVertex);
    if (!vertexSpirV.has_value())
    {
        throw std::runtime_error("Failed to create shader, shader '" + a_identifier.toString() + ".vert' Couldn't be compiled");
    }
    m_vertexSpirV = vertexSpirV.value();
    m_vkVertexShaderCreateInfo.codeSize = m_vertexSpirV.size() * sizeof(uint32_t);
    m_vkVertexShaderCreateInfo.pCode = m_vertexSpirV.data();
}

void GraphicsPipeline::setVkDevice(const vk::raii::Device &a_device, const vk::Extent2D &a_vkSwapExtent)
{
    const vk::raii::ShaderModule fragmentShader(a_device, m_vkFragmentShaderCreateInfo);
    const vk::raii::ShaderModule vertexShader(a_device, m_vkVertexShaderCreateInfo);

    const vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = fragmentShader,
        .pName = "main"
    };

    const vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = vertexShader,
        .pName = "main"
    };

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    vk::DynamicState dynamicStates[] = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{
        .dynamicStateCount = std::size(dynamicStates),
        .pDynamicStates = dynamicStates
    };

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
        .topology = vk::PrimitiveTopology::eTriangleList
    };

    vk::PipelineViewportStateCreateInfo viewportState{
        .viewportCount = 1,
        .scissorCount = 1
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer{
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eClockwise,
        .depthBiasEnable = vk::False,
        .depthBiasSlopeFactor = 1.0f,
        .lineWidth = 1.0f
    };

    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False
    };
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = vk::False,
        .srcColorBlendFactor = vk::BlendFactor::eSrcColor,
        .dstColorBlendFactor = vk::BlendFactor::eDstColor,
        .colorBlendOp = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eSrcAlpha,
        .dstAlphaBlendFactor = vk::BlendFactor::eDstAlpha,
        .alphaBlendOp = vk::BlendOp::eAdd,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    };

    vk::PipelineColorBlendStateCreateInfo colorBlending{
        .logicOpEnable = vk::False,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0
    };

    m_vkPipelineLayout = vk::raii::PipelineLayout(a_device, pipelineLayoutInfo);

    vk::Viewport pipelineRenderingCreateInfo{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(a_vkSwapExtent.width),
        .height = static_cast<float>(a_vkSwapExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    vk::GraphicsPipelineCreateInfo pipelineInfo{
        .pNext = &pipelineRenderingCreateInfo,
        .stageCount = std::size(shaderStages),
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = m_vkPipelineLayout,
        .renderPass = nullptr
    };

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    m_vkGraphicsPipeline = vk::raii::Pipeline(a_device, nullptr, pipelineInfo);
}
