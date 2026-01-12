#pragma once

#include <algorithm>
#include <format>
#include <optional>
#include <string>
#include <utility>

using std::string;

namespace Utils
{
    inline const string VANILLA_NAMESPACE = "vanilla";

    class Identifier final
    {
    public:
        [[nodiscard]]
        static std::optional<Identifier> of(const string &a_namespace, const string &a_path);

        [[nodiscard]]
        static std::optional<Identifier> ofVanilla(const string &a_path);

        [[nodiscard]]
        static std::optional<Identifier> of(const string &a_string);

        [[nodiscard]]
        static std::optional<Identifier> parse(const string &a_string);

        [[nodiscard]]
        static Identifier ofUnsafe(const string &a_namespace, const string &a_path);

        [[nodiscard]]
        static bool isNamespaceValid(const string &a_namespace);

        [[nodiscard]]
        static bool isNamespaceCharValid(const char c);

        [[nodiscard]]
        static bool isPathValid(const string &a_path);

        [[nodiscard]]
        static bool isPathCharValid(const char c);

        [[nodiscard]]
        string getNamespace() const;

        [[nodiscard]]
        string getPath() const;

        [[nodiscard]]
        Identifier withNamespace(const string &a_namespace) const;

        [[nodiscard]]
        Identifier withPath(const string &a_path) const;

        [[nodiscard]]
        Identifier withPrefixedPath(const string &a_prefix) const;

        [[nodiscard]]
        Identifier withSuffixedPath(const string &a_suffix) const;

        [[nodiscard]]
        string toString() const;

    private:
        Identifier(string a_namespace, string a_path)
            : m_namespace(std::move(a_namespace)), m_path(std::move(a_path)) {}

        const string m_namespace;
        const string m_path;
    };
}
