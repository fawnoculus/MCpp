#include "Identifier.h"

using Utils::Identifier;

[[nodiscard]]
std::optional<Identifier> Identifier::of(const string &a_namespace, const string &a_path)
{
    if (isNamespaceValid(a_namespace) && isPathValid(a_path))
    {
        return Identifier(a_namespace, a_path);
    }
    return {};
}

[[nodiscard]]
std::optional<Identifier> Identifier::ofVanilla(const string &a_path)
{
    if (isPathValid(a_path))
    {
        return Identifier(VANILLA_NAMESPACE, a_path);
    }
    return {};
}

[[nodiscard]]
std::optional<Identifier> Identifier::of(const string &a_string)
{
    if (const size_t i = a_string.find(':'); i != string::npos)
    {
        const string string1 = a_string.substr(i + 1);
        if (i != 0)
        {
            return of(a_string.substr(0, i), string1);
        }
        return ofVanilla(string1);
    }
    return ofVanilla(a_string);
}

[[nodiscard]]
std::optional<Identifier> Identifier::parse(const string &a_string)
{
    if (const size_t i = a_string.find(':'); i != string::npos && i != 0)
    {
        return of(a_string.substr(0, i), a_string.substr(i + 1));
    }
    return {};
}

[[nodiscard]]
Identifier Identifier::ofUnsafe(const string &a_namespace, const string &a_path)
{
    return {a_namespace, a_path};
}

[[nodiscard]]
bool Identifier::isNamespaceValid(const string &a_namespace)
{
    return std::ranges::all_of(a_namespace, &isNamespaceCharValid);
}

[[nodiscard]]
bool Identifier::isNamespaceCharValid(const char c)
{
    return c == '_' || c == '-' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' || c == '.';
}

[[nodiscard]]
bool Identifier::isPathValid(const string &a_path)
{
    return std::ranges::all_of(a_path, &isPathCharValid);
}

[[nodiscard]]
bool Identifier::isPathCharValid(const char c)
{
    return c == '_' || c == '-' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' || c == '/' || c == '.';
}

[[nodiscard]]
string Identifier::getNamespace() const
{
    return this->m_path;
}

[[nodiscard]]
string Identifier::getPath() const
{
    return this->m_path;
}

[[nodiscard]]
Identifier Identifier::withNamespace(const string &a_namespace) const
{
    return {a_namespace, this->getPath()};
}

[[nodiscard]]
Identifier Identifier::withPath(const string &a_path) const
{
    return {this->getNamespace(), a_path};
}

[[nodiscard]]
Identifier Identifier::withPrefixedPath(const string &a_prefix) const
{
    return withPath(a_prefix + this->getPath());
}

[[nodiscard]]
Identifier Identifier::withSuffixedPath(const string &a_suffix) const
{
    return withPath(this->getPath() + a_suffix);
}

[[nodiscard]]
string Identifier::toString() const
{
    return std::format("{}:{}", m_namespace, m_path);
}
