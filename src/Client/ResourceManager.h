#pragma once
#include <filesystem>
#include <vulkan/vulkan_raii.hpp>

#include "Utils.h"
#include "spdlog/spdlog.h"

class ResourceManager
{
public:
    ResourceManager() = default;

    ResourceManager(std::nullptr_t) {}

    explicit ResourceManager(const std::shared_ptr<spdlog::logger> &a_logger, const std::filesystem::path &a_resourceDirectory);

    // Used to create the Texture Atlas TODO
    void setVkDevice(const vk::raii::Device &a_vkDevice);

    template<class T>
    std::optional<std::basic_ifstream<T>> getResourceStream(const Utils::Identifier &a_identifier);

private:
    std::shared_ptr<spdlog::logger> m_logger = nullptr;
    std::filesystem::path m_resourceDirectory;
};
