#pragma once

#include "spdlog/spdlog.h"
#include "vulkan/vulkan_raii.hpp"

#include "Utils/Identifier.h"
#include "ResourceManager.h"

using std::optional;

class GraphicsPipeline final
{
public:
    GraphicsPipeline() = delete;

    GraphicsPipeline(std::nullptr_t) {}

    explicit GraphicsPipeline(const std::shared_ptr<spdlog::logger> &a_logger, const ResourceManager &resourceManager, const Utils::Identifier &a_identifier);

    void setVkDevice(const vk::raii::Device &a_device, const vk::Extent2D &a_vkSwapExtent);

private:
    std::shared_ptr<spdlog::logger> m_logger = nullptr;
    std::vector<uint32_t> m_vertexSpirV;
    std::vector<uint32_t> m_fragmentSpirV;
    vk::ShaderModuleCreateInfo m_vkFragmentShaderCreateInfo;
    vk::ShaderModuleCreateInfo m_vkVertexShaderCreateInfo;
    vk::raii::PipelineLayout m_vkPipelineLayout = nullptr;
    vk::raii::Pipeline m_vkGraphicsPipeline = nullptr;
};
