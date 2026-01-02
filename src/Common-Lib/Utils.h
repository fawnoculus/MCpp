#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <utility>

using std::string;

namespace Utils {
    static const string VANILLA_NAMESPACE = "vanilla";
    class Identifier final {
    public:
        static constexpr std::optional<Identifier> of(const string &a_namespace, const string &a_path) {
            if (isNamespaceValid(a_namespace) && isPathValid(a_path)) {
                return Identifier(a_namespace, a_path);
            }
            return {};
        }

        static constexpr std::optional<Identifier> ofVanilla(const string &a_path) {
            if (isPathValid(a_path)) {
                return Identifier(VANILLA_NAMESPACE, a_path);
            }
            return {};
        }

        static constexpr std::optional<Identifier> of(const string &a_string) {
            if (const size_t i = a_string.find(':'); i != string::npos) {
                const string string1 = a_string.substr(i + 1);
                if (i != 0) {
                    return of(a_string.substr(0, i), string1);
                }
                return ofVanilla(string1);
            }
            return ofVanilla(a_string);
        }

        static constexpr std::optional<Identifier> parse(const string &a_string) {
            if (const size_t i = a_string.find(":"); i != string::npos && i != 0) {
                return of(a_string.substr(0, i), a_string.substr(i + 1));
            }
            return {};
        }

        static constexpr Identifier ofUnsafe(const string &a_namespace, const string &a_path) {
            return {a_namespace, a_path};
        }

        [[nodiscard]]
        constexpr string getNamespace() const {
            return this->m_path;
        }

        [[nodiscard]]
        constexpr string getPath() const {
            return this->m_path;
        }

        [[nodiscard]]
        constexpr Identifier withNamespace(const string &a_namespace) const {
            return {a_namespace, this->getPath()};
        }

        [[nodiscard]]
        constexpr Identifier withPath(const string &a_path) const {
            return {this->getNamespace(), a_path};
        }

        [[nodiscard]]
        constexpr Identifier withPrefixedPath(const string &a_prefix) const {
            return withPath(a_prefix + this->getPath());
        }

        [[nodiscard]]
        constexpr Identifier withSuffixedPath(const string &a_suffix) const {
            return withPath(this->getPath() + a_suffix);
        }

        static constexpr bool isNamespaceValid(const string &a_namespace) {
            return std::ranges::all_of(a_namespace, isNamespaceCharValid);
        }

        static constexpr bool isNamespaceCharValid(const char c) {
		    return c == '_' || c == '-' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' || c == '.';
        }

        static constexpr bool isPathValid(const string &a_path) {
            return std::ranges::all_of(a_path, isPathCharValid);
        }

        static constexpr bool isPathCharValid(const char c) {
            return c == '_' || c == '-' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' || c == '/' || c == '.';
        }

    private:
        Identifier(string a_namespace, string a_path)
            : m_namespace(std::move(a_namespace)), m_path(std::move(a_path)) {
        }
        const string m_namespace;
        const string m_path;
    };
}
