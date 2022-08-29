#include "shaders/VertexAttribute.hpp"
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
        "{{ name: {}, binding: {}, location: {}, vecsize: {} }}", val.name, val.binding, val.location, val.vecsize);
}

std::ostream& operator<<(std::ostream& out, const VertexAttribute& val)
{
    std::visit(Overload{[&out](const auto& val) { toString(out, val); }}, val);
    return out;
}
} // namespace renderingEngine