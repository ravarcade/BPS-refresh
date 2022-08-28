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
    out << fmt::format(
        "{{ name: {}, vecsize: {}, columns: {}, array: {}, offset: {}, size: {} }}",
        val.name,
        val.vecsize,
        val.columns,
        val.array,
        val.offset,
        val.size);
}

template <>
void toString(std::ostream& out, const SvUbo& val)
{
    out << fmt::format(
        "{{ ubo: {}, set: {}, binding: {}, size: {}, members: {} }}",
        val.name,
        val.set,
        val.binding,
        val.size,
        val.members);
}

template <>
void toString(std::ostream& out, const SvStruct& val)
{
    out << fmt::format(
        "{{ name: {}, vecsize: {}, columns: {}, array: {}, offset: {}, size: {}, members: {} }}",
        val.name,
        val.vecsize,
        val.columns,
        val.array,
        val.offset,
        val.size,
        val.members);
}

template <>
void toString(std::ostream& out, const SvPushConst& val)
{
    out << fmt::format(
        "{{ name: {}, set: {}, binding: {}, offset: {}, size: {}, members: {} }}",
        val.name,
        val.set,
        val.binding,
        val.offset,
        val.size,
        val.members);
}

template <>
void toString(std::ostream& out, const SvSampler& val)
{
    out << fmt::format(
        "{{ name: {}, set: {}, binding: {}, dim: {}, stage: {} }}", val.name, val.set, val.binding, val.dim, val.stage);
}

std::ostream& operator<<(std::ostream& out, const ShaderVariable& val)
{
    std::visit(Overload{[&out](const auto& val) { toString(out, val); }}, val);
    return out;
}
} // namespace renderingEngine