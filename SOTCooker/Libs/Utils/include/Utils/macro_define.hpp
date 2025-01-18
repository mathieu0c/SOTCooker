#pragma once

#include <array>
#include <cstdint>
#include <iostream>

/* --- Enum declarations --- */

/*! @brief Usage example: DECLARE_CUSTOM_ENUM(AvailableGearHandlers, kTheCrew2, kTheCrewMotorfest); */
#define DECLARE_CUSTOM_ENUM(EnumName, ...)                                                                 \
  enum class EnumName : int32_t { __VA_ARGS__, kMaxEnumValue };                                            \
  inline std::ostream& operator<<(std::ostream& os, EnumName value) {                                      \
    static constexpr std::array<const char*, static_cast<size_t>(EnumName::kMaxEnumValue) + 1> strings = { \
        #__VA_ARGS__, "kMaxEnumValue"};                                                                    \
    return os << strings[static_cast<std::underlying_type<EnumName>::type>(value)];                        \
  }

/*! @brief Usage example: DECLARE_ENUM_DISPLAY_NAMES(AvailableGearHandlers, "The Crew 2", "The Crew Motorfist"); */
#define DECLARE_ENUM_DISPLAY_NAMES(EnumName, ...)                                                          \
  inline auto& Get##EnumName##Names() {                                                                    \
    static constexpr std::array<const char*, static_cast<size_t>(EnumName::kMaxEnumValue) + 1> strings = { \
        __VA_ARGS__};                                                                                      \
    return strings;                                                                                        \
  }                                                                                                        \
  inline EnumName NameTo##EnumName(const char* name) {                                                     \
    for (size_t i{}; i < Get##EnumName##Names().size(); ++i) {                                             \
      if (strcmp(Get##EnumName##Names()[i], name) == 0) {                                                  \
        return static_cast<EnumName>(i);                                                                   \
      }                                                                                                    \
    }                                                                                                      \
    return EnumName::kMaxEnumValue;                                                                        \
  }                                                                                                        \
  inline EnumName NameTo##EnumName(const QString& name) {                                                  \
    return NameTo##EnumName(qPrintable(name));                                                             \
  }
