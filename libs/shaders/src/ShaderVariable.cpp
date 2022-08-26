#include "shaders/ShaderVariable.hpp"
#include "common/Logger.hpp"

namespace renderingEngine
{
template <typename... Ts>
struct Overload : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
Overload(Ts...) -> Overload<Ts...>;

template <typename T>
void toString(std::ostream& out, const T& val)
{
    out << fmt::format("{{ name: {}, vecsize: {}, columns: {} }}", val.name, val.vecsize, val.columns);
}

template <>
void toString(std::ostream& out, const SvUbo& val)
{
    out << fmt::format("{{ ubo: {}, members: {} }}", val.name, val.members);
}

template <>
void toString(std::ostream& out, const SvStruct& val)
{
    out << fmt::format(
        "{{name: {}, vecsize: {}, columns: {}, members: {} }}", val.name, val.vecsize, val.columns, val.members);
}

std::ostream& operator<<(std::ostream& out, const ShaderVariable& val)
{
    std::visit(Overload{[&out](const auto& val) { toString(out, val); }}, val);
    return out;
}
} // namespace renderingEngine