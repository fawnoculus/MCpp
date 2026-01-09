#pragma once

#include "spdlog/spdlog.h"
#include "vulkan/vulkan_raii.hpp"

#include <Utils.h>

using std::optional;

class GraphicsPipeline final
{
public:
    GraphicsPipeline() = delete;

    GraphicsPipeline(std::nullptr_t) {}

    explicit GraphicsPipeline(const Utils::Identifier& a_identifier);

    void setVkDevice(const vk::raii::Device& a_device);

private:
    optional<vk::ShaderModuleCreateInfo> m_vkVertexShaderCreateInfo = {};
    optional<vk::ShaderModuleCreateInfo> m_vkTessellationShaderCreateInfo = {};
    optional<vk::ShaderModuleCreateInfo> m_vkFragmentShaderCreateInfo = {};
};
