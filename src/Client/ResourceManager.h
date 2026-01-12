#pragma once
#include <filesystem>

#include "spdlog/spdlog.h"
#include "vulkan/vulkan_raii.hpp"
#include "glslang/Public/ShaderLang.h"

#include "Utils/Identifier.h"

class ResourceManager final
{
public:
    ResourceManager() = default;

    ResourceManager(std::nullptr_t) {}

    explicit ResourceManager(const std::shared_ptr<spdlog::logger> &a_logger, const std::filesystem::path &a_resourceDirectory);

    // Used to create the Texture Atlas TODO
    void setVkDevice(const vk::raii::Device &a_vkDevice);

    template<typename T>
    [[nodiscard]]
    std::optional<std::basic_ifstream<T>> getResourceStream(const Utils::Identifier &a_identifier) const;

    [[nodiscard]]
    std::optional<std::vector<uint32_t>> getCompiledShader(const Utils::Identifier &a_identifier, EShLanguage stage) const;

private:
    std::shared_ptr<spdlog::logger> m_logger = nullptr;
    std::filesystem::path m_resourceDirectory;
};
