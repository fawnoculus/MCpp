#include "ResourceManager.h"

using std::string;

bool isResourceDirectoryValid(const std::filesystem::path &a_path)
{
    return std::filesystem::is_directory(a_path / "vanilla" / "assets")
           && std::filesystem::is_directory(a_path / "vanilla" / "lang")
           && std::filesystem::is_directory(a_path / "vanilla" / "shaders");
}

ResourceManager::ResourceManager(const std::shared_ptr<spdlog::logger> &a_logger, const std::filesystem::path &a_resourceDirectory)
    : m_logger(a_logger), m_resourceDirectory(a_resourceDirectory)
{
    if (!isResourceDirectoryValid(a_resourceDirectory))
    {
        m_logger->error("Resource directory '{}' does not contain a valid directory structure, things will probably not go well", a_resourceDirectory.string());
    }
}

void ResourceManager::setVkDevice(const vk::raii::Device &a_vkDevice) {}

template<typename T>
std::optional<std::basic_ifstream<T> > ResourceManager::getResourceStream(const Utils::Identifier &a_identifier)
{
    std::filesystem::path path = m_resourceDirectory / a_identifier.getPath();

    std::stringstream namespaceStream(a_identifier.getNamespace());
    string pathPart;

    while (getline(namespaceStream, pathPart, '/'))
        path /= pathPart;

    if (!std::filesystem::is_regular_file(path))
        return {};

    return std::basic_ifstream<T>(path);
}
