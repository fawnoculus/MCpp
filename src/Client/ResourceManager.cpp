#include <expected>
#include <fstream>

#include "glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"

#include "ResourceManager.h"

using std::vector, std::expected, std::string, std::optional;

glslang::SpvOptions g_spvOptions{
    .generateDebugInfo = true
};

[[nodiscard]]
expected<vector<uint32_t>, string> compileGLSLShader(const EShLanguage stage, const std::string &glslCode)
{}

[[nodiscard]]
bool isResourceDirectoryValid(const std::filesystem::path &a_path)
{
    return std::filesystem::is_directory(a_path / "assets" / "vanilla" / "lang")
           && std::filesystem::is_directory(a_path / "assets" / "vanilla" / "shaders")
           && std::filesystem::is_directory(a_path / "assets" / "vanilla" / "textures");
}

ResourceManager::ResourceManager(const std::shared_ptr<spdlog::logger> &a_logger, const std::filesystem::path &a_resourceDirectory)
    : m_logger(a_logger), m_resourceDirectory(a_resourceDirectory)
{
    if (!isResourceDirectoryValid(a_resourceDirectory))
    {
        m_logger->error("Resource directory '{}' does not contain a valid directory structure, things will probably not go well", a_resourceDirectory.string());
    }
}

void ResourceManager::setVkDevice(const vk::raii::Device &a_vkDevice)
{
    // TODO: make Texture Atlas
}

template<typename T>
[[nodiscard]]
optional<std::basic_ifstream<T>> ResourceManager::getResourceStream(const Utils::Identifier &a_identifier) const
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

[[nodiscard]]
optional<vector<uint32_t>> ResourceManager::getCompiledShader(const Utils::Identifier &a_identifier, const EShLanguage stage) const
{
    auto stream = getResourceStream<char>(a_identifier.withPrefixedPath("shaders"));
    if (!stream.has_value())
        return {};

    const vector chars(std::istreambuf_iterator(stream.value()), std::istreambuf_iterator<char>());

    glslang::TShader shader{
        stage
    };
    shader.setDebugInfo(true);

    const char *str = chars.data();
    const int length = static_cast<int>(chars.size());
    shader.setStringsWithLengths(&str, &length, 1);

    shader.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetClientVersion::EShTargetVulkan_1_3);
    shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_6);
    shader.setEnvInput(
        glslang::EShSource::EShSourceGlsl,
        stage,
        glslang::EShClient::EShClientOpenGL,
        glslang::EShTargetClientVersion::EShTargetOpenGL_450
    );
    shader.setSourceEntryPoint("main");

    if (auto includer = glslang::TShader::ForbidIncluder{};
        !shader.parse(
            nullptr,
            glslang::EShTargetClientVersion::EShTargetOpenGL_450,
            false,
            EShMessages::EShMsgDefault,
            includer
        )
    )
    {
        m_logger->warn("Failed to parse shader '{}'\nShader info log:\n{}\nDebug log:\n{}\n",
                       a_identifier.toString(), shader.getInfoLog(), shader.getInfoDebugLog()
        );
    }

    vector<uint32_t> spirV{};
    glslang::GlslangToSpv(*shader.getIntermediate(), spirV, &g_spvOptions);

    return spirV;
}
