#include <type_traits>
#include <fmt/format.h>

template <typename EnumType>
requires std::is_enum_v<EnumType>
struct fmt::formatter<EnumType> : fmt::formatter<std::underlying_type_t<EnumType>>
{
    // Forwards the formatting by casting the enum to it's underlying type
    auto format(const EnumType& enumValue, format_context& ctx) const
    {
        return fmt::formatter<std::underlying_type_t<EnumType>>::format(
            static_cast<std::underlying_type_t<EnumType>>(enumValue), ctx);
    }
};